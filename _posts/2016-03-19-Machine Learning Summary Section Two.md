---
layout: post
title: "Machine Learning Summary Section Two"
author: Zdf
tag: [Machine Learning]
category: Machine Learning
date: 2016-03-19 22:01:22 +0800
comments: true
---

[Last Section](http://zdf615328619.github.io/machine%20learning/2016/01/13/Machine-Learning-Summary-Section-One/)

### 多变量线性回归 _Linear Regression with Multiple Variable_

* $h_{\theta}(x)=\theta_{0} + \theta_{1}x_{1} + \theta_{2}x_{2} + ... + \theta_{n}x_{n}$
* $x_{0} = 1$ , $h_{\theta}(x)=\theta_{0}x_{0} + \theta_{1}x_{1} + \theta_{2}x_{2} + ... + \theta_{n}x_{n}$
* $h_{\theta}(x)=\theta^{T}X$

### 特征缩放 _Feature scaling_

* make sure features are on a similar scale
* get every feature into approximately a $-1\leq x_{i} \leq +1$ range
* mean normalization 均值归一化 $ x_{i} = \frac{x_{i} - \mu_{i}}{s_{i}} $ . ($ \mu_{i} $ is avg and $ s_{i} $ can be $ max - min $ or standard deviation )

### 学习率 _Learning rate_

* making sure gradient descent is working correctly. $ J(\theta) $ should decrease after every iteration.
* $ J(\theta)-iteration $ 曲线上升或波动，需要降低学习率。学习率要适中，过小会导致梯度下降的太慢
* To try $ \alpha $, try ... 0.001, 0.003, 0.01, 0.03, 0.1, 0.3, 1, 3 ...