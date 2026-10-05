---
layout: post
title: "Finding Videos in Chrome's Cache"
tags: [Chrome, video]
categories: tricks
date: 2016-02-21 13:15:10 +0800
---

##### How to find cached videos in Chrome and download them

* Under `C:\Users\Dongfang\AppData\Local\Google\Chrome\User Data\Default\Pepper Data\Shockwave Flash` you'll find `.tmp` files — copy one and change the extension (the cache is deleted once you close the video page)
* Press F12, or right-click and choose "Inspect" to open DevTools — you'll find the video URL under Network; download it with any download tool (you can also see the file extension there)
* Or use a browser extension — I used `FVD Video Downloader`

Note: `Dongfang` is the Windows username

_Note: when publishing a post, the date has to make sense, otherwise it won't show up on the blog_