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

### 特征与多项式回归 _Features and Polynomial Regression_

线性回归并不能拟合所有数据，我们要使用其他模型来进行拟合

### 正规方程 _Normal Equation_

* 对于某些线性回归问题，我们可以使用正规方程来解决。复杂度为 $O(n^{3})$
* 训练集特征向量$X$(其中$x_{0} = 1$), 训练集结果为向量y
* 利用正规方程解出向量$$ \theta = (X^{T}X)^{-1}X^{T}y $$
* $X^{T}X$ 是为了构成方阵，只有方程才有逆。$X\theta = y$

### Octave or Matlab

* 基本操作
* 移动数据
* 计算数据
* 绘图数据
* 控制语句与函数
* 向量化

### 逻辑回归 _Logical Regression_

* 逻辑回归模型的假设 $h_{\theta}(x) = g(\theta_{T}X)$
* Sigmod Function $ g(z) = \frac{1}{1 + e^{-z}} $
* 逻辑回归的代价函数 $$J(\theta) = \frac{1}{m} \sum_{i = 1}^{m}Cost(h_\theta(x^{(i)}),y^{(i)}) $$
* 化简后 $$ Cost(h_\theta(x),y) = -y \times log(h_\theta(x)) - (1-y) \times log(1 - h_\theta(x))$$
* fminunc
* one-vs-all 取 $max(h_\theta(x))$ 为分类结果

[Next Section](http://zdf615328619.github.io/machine%20learning/2016/03/23/Machine-Learning-Summary-Section-Three/)
