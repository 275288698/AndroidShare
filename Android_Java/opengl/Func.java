http://blog.csdn.net/shizhipeng/article/details/4939529
	glViewport()函数和glOrtho()函数的理解
	lOrtho(left, right, bottom, top, near, far)，
	left表示视景体左面的坐标，right表示右面的坐标，bottom表示下面的，top表示上面的。
	这个函数简单理解起来，就是一个物体摆在那里，你怎么去截取他。这里，我们先抛开glViewport函数不看。
	先单独理解glOrtho的功能。 
	假设有一个球体，半径为1，圆心在(0, 0, 0)，那么，

我们设定glOrtho(-1.5, 1.5, -1.5, 1.5, -10, 10);
就表示用一个宽高都是3的框框把这个球体整个都装了进来。 
如果设定glOrtho(0.0, 1.5, -1.5, 1.5, -10, 10);
就表示用一个宽是1.5， 高是3的框框把整个球体的右面装进来;
如果设定glOrtho(0.0, 1.5, 0.0, 1.5, -10, 10)；
就表示用一个宽和高都是1.5的框框把球体的右上角装了进来