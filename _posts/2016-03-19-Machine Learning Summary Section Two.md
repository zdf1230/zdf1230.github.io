---
layout: post
title: "Machine Learning Summary Section Two"
tags: [Machine Learning]
categories: Machine Learning
date: 2016-03-19 22:01:22 +0800
---

[Last Section](/machine%20learning/2016/01/13/Machine-Learning-Summary-Section-One/)

### Linear Regression with Multiple Variables

* $h_{\theta}(x)=\theta_{0} + \theta_{1}x_{1} + \theta_{2}x_{2} + ... + \theta_{n}x_{n}$
* $x_{0} = 1$ , $h_{\theta}(x)=\theta_{0}x_{0} + \theta_{1}x_{1} + \theta_{2}x_{2} + ... + \theta_{n}x_{n}$
* $h_{\theta}(x)=\theta^{T}X$

### Feature Scaling

* make sure features are on a similar scale
* get every feature into approximately a $-1\leq x_{i} \leq +1$ range
* mean normalization $ x_{i} = \frac{x_{i} - \mu_{i}}{s_{i}} $ . ($ \mu_{i} $ is avg and $ s_{i} $ can be $ max - min $ or standard deviation )

### Learning Rate

* making sure gradient descent is working correctly. $ J(\theta) $ should decrease after every iteration.
* $ J(\theta)-iteration $ curve goes up or oscillates, lower the learning rate. The learning rate should be moderate — too small makes gradient descent painfully slow
* To try $ \alpha $, try ... 0.001, 0.003, 0.01, 0.03, 0.1, 0.3, 1, 3 ...

### Features and Polynomial Regression

Linear regression can't fit all data; we need other models

### Normal Equation

* For some linear regression problems, we can use the normal equation. Complexity: $O(n^{3})$
* Training-set feature matrix $X$ (with $x_{0} = 1$); training results as vector y
* Solve for the vector with the normal equation:$$ \theta = (X^{T}X)^{-1}X^{T}y $$
* $X^{T}X$ forms a square matrix, which is what makes it invertible.$X\theta = y$

### Octave or Matlab

* Basic operations
* Moving data
* Computing on data
* Plotting data
* Control statements and functions
* Vectorization

### Logistic Regression

* Hypothesis of the logistic regression model: $h_{\theta}(x) = g(\theta_{T}X)$
* Sigmoid function: $ g(z) = \frac{1}{1 + e^{-z}} $
* Cost function of logistic regression: $$J(\theta) = \frac{1}{m} \sum_{i = 1}^{m}Cost(h_\theta(x^{(i)}),y^{(i)}) $$
* Simplified: $$ Cost(h_\theta(x),y) = -y \times log(h_\theta(x)) - (1-y) \times log(1 - h_\theta(x))$$
* fminunc
* one-vs-all: take $max(h_\theta(x))$ as the classification result

### Regularization
* Overfitting — high variance
* Solutions to overfitting
	* Reduce the number of features (manually select which features to keep, or use a model selection algorithm)
	* Regularization
* Applying regularization to linear and logistic regression: $\lambda$

[Next Section](/machine%20learning/2016/03/23/Machine-Learning-Summary-Section-Three/)
