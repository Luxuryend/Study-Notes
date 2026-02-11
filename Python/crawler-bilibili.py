import asyncio
import aiohttp

async def search(keyword):
    url = "https://api.bilibili.com/x/web-interface/search/all/v2"
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
        "Referer": "https://www.bilibili.com"
    }
    params = {"keyword": keyword}

    async with aiohttp.ClientSession() as session:
        async with session.get(url, headers=headers, params=params) as resp:
            content = (await resp.json())["data"]["result"][-1]["data"]

    infos = []
    for item in content:
        title = (item["title"]).replace('<em class="keyword">', '').replace('</em>', '')
        infos.append([title, item["duration"], item["bvid"]])
    return infos

if __name__ == '__main__':
    from pprint import pprint
    a = asyncio.run(search('daylight'))
    pprint(a)
