import re
import os
import asyncio
import aiohttp
import aiofiles
import time
from functools import wraps

def timer_sync(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        print('Synchronous timer decorator')
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        print(f'函数{func.__name__}耗时 : {end_time-start_time}秒')
        return result
    return wrapper

def timer_async(func):
    @wraps(func)
    async def wrapper(*args, **kwargs):
        print('Asynchronous timer decorator')
        start_time = time.time()
        result = await func(*args, **kwargs)
        end_time = time.time()
        print(f'函数{func.__name__}耗时 : {end_time - start_time}秒')
        return result
    return wrapper


class BilibiliCrawler:

    audios_folder = 'audios'
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36',
        'Referer': 'https://www.bilibili.com'
    }
    search_api = 'https://api.bilibili.com/x/web-interface/search/all/v2'

    def __init__(self):
        if not os.path.exists(self.audios_folder):
            os.mkdir(self.audios_folder)
            print('audio folder does not exist, automatically created')
        self.async_session = None

    async def create_session(self):
        if self.async_session is None:
            self.async_session = aiohttp.ClientSession(headers=self.headers)

    async def delete_session(self):
        if self.async_session:
            await self.async_session.close()
            self.async_session = None



    @timer_async
    async def search(self, keyword: str) -> list:
        # Bilibili search api call
        params = {"keyword": keyword}
        async with self.async_session.get(self.search_api, params=params) as r:
            content = (await r.json())["data"]["result"][-1]["data"]
        infos = []
        for item in content:
            title = (item["title"]).replace('<em class="keyword">', '').replace('</em>', '')
            infos.append([title, item["duration"], item["bvid"]])
        return infos


    @timer_async
    async def download_audio(self, bvid: str, title: str) -> None:
        # download audio file from Bilibili video
        url = 'https://www.bilibili.com/video/' + bvid
        async with self.async_session.get(url) as r:
            html_text = await r.text()
            match = re.search(r'"audio"(?:.*?)"baseUrl":"(.*?)"](.*?)"audio/mp4"', html_text)
            audio_url = match.group(1)
        async with self.async_session.get(audio_url) as r:
            async with aiofiles.open(self.audios_folder + "/" + title + ".mp3", "wb") as f:
                audio_data = await r.read()
                await f.write(audio_data)



if __name__ == '__main__':
    async def main():
        b = BilibiliCrawler()
        await b.create_session()
        try:
            results = await b.search('daylight')
            print(results)
            await b.download_audio('BV1um4y1H7DU', 'yi《Daylight》')
        finally:
            await b.delete_session()

    asyncio.run(main())
