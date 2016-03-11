---
layout: post
title: "Machine Learning Summary Section One"
author: Zdf
tag: [Machine Learning]
category: Machine Learning
date: 2016-01-13 16:34:10 -0700
comments: true
---

### 机器学习的定义
由Tom Mitchell提出，来自卡内基梅隆大学，Tom定义的机器学习是，一个好的学习问题定义如下，他说，一个程序被认为能从经验E中学习，解决任务T，达到性能度量值P，当且仅当，有了经验E后，经过P评判，程序在处理T时的性能有所提升。

### 监督学习 _Supervised Learning_
* 回归 _Regression_ 
    * predict continuous valued output
* 分类 _Classification_
	* discrete valued output

### 无监督学习 _Unsupervised Learning_
* 聚类
* 鸡尾酒宴会问题
	* $$[W,s,v] = svd((repmat(sum(x.*x,1),size(x,1),1).*x)*x');$$
	
### 单变量线性回归 _Linear Regression with One Variable_

* 训练集 _Training Set_
* 假设 _hypothesis_
* 代价函数 _Cost Funciton_
* 梯度下降 _Gradient Descent_

Example:

Hypothesis : $$h_{\theta}(x)={\theta}_1 + {\theta}_2 \times x$$

Cost Funciton : $$J(\theta_1,\theta_2)=\frac{1}{2m}\sum_{i = 1}^{m}(h_\theta(x^{(i)})-y^{(i)})^2$$


### 批量梯度下降算法 _batch gradient descent algorithm_

* 算法
    
    For the upper example, it has

    repeat until convergence {

    $$\theta_j := \theta_j - \alpha \frac{\partial}{\partial\theta_j} J(\theta_0,\theta_1)$$

    }
    
    $\alpha$ 是学习率 _learning rate_

* 局部最优
    * $\theta$ 最终会收敛，偏导数为 $0$
	
### 多变量线性回归 _Linear Regression with Multiple Variable_

* $h_{\theta}(x)=\theta_{0} + \theta_{1}x_{1} + \theta_{2}x_{2} + ... + \theta_{n}x_{n}$
* $x_{0} = 1$ , $h_{\theta}(x)=\theta_{0}x_{0} + \theta_{1}x_{1} + \theta_{2}x_{2} + ... + \theta_{n}x_{n}$
* $h_{\theta}(x)=\theta^{T}X$
