import requests

def searchBilibili(keyword : str) -> list:
    headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36',
        'Referer': 'https://www.bilibili.com'
    }
    search_api = 'https://api.bilibili.com/x/web-interface/search/all/v2'
    params = {"keyword": keyword}
    r = requests.get(search_api, params=params, headers=headers)

    content = r.json()["data"]["result"][-1]["data"]

    infos = []
    for item in content:
        title = (item["title"]).replace('<em class="keyword">', '').replace('</em>', '')
        infos.append([title, item["author"], item["duration"], item["bvid"]])

    return infos


if __name__ == '__main__':
    infos = searchBilibili("M3")


