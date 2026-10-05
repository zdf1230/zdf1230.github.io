---
layout: post
title: "Machine Learning Summary Section One"
tags: [Machine Learning]
categories: Machine Learning
date: 2016-01-13 16:34:10 -0700
---

### Definition of Machine Learning
Proposed by Tom Mitchell of Carnegie Mellon University. His definition: a program is said to learn from experience E with respect to some task T and performance measure P, if its performance on T, as measured by P, improves with experience E.

### Supervised Learning
* Regression 
    * predict continuous valued output
* Classification
	* discrete valued output

### Unsupervised Learning
* Clustering
* The cocktail party problem
	* $$[W,s,v] = svd((repmat(sum(x.*x,1),size(x,1),1).*x)*x');$$
	
### Linear Regression with One Variable

* Training Set
* Hypothesis
* Cost Function
* Gradient Descent

Example:

Hypothesis : $$h_{\theta}(x)={\theta}_1 + {\theta}_2 \times x$$

Cost Funciton : $$J(\theta_1,\theta_2)=\frac{1}{2m}\sum_{i = 1}^{m}(h_\theta(x^{(i)})-y^{(i)})^2$$


### Batch Gradient Descent Algorithm

* The algorithm
    
    For the upper example, it has

    repeat until convergence {

    $$\theta_j := \theta_j - \alpha \frac{\partial}{\partial\theta_j} J(\theta_0,\theta_1)$$

    }
    
    $\alpha$ is the learning rate

* Local optimum
    * $\theta$ will eventually converge, with partial derivatives equal to $0$
	
[Next Section](/machine/learning/2016/03/19/Machine-Learning-Summary-Section-Two/)
