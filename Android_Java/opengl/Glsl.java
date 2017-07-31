http://blog.csdn.net/misol/article/details/7658949
	http://blog.csdn.net/racehorse/article/details/6634830

public class Glsl {
	private final String vertexShaderCode =
		    "attribute vec4 vPosition;" +
		    "void main() {" +
		    "  gl_Position = vPosition;" +
		    "}";

		private final String fragmentShaderCode =
		    "precision mediump float;" +
		    "uniform vec4 vColor;" +
		    "void main() {" +
		    "  gl_FragColor = vColor;" +
		    "}";
		    
		   }
}

OpenGL ES着色器语言之变量和数据类型
http://blog.csdn.net/hgl868/article/details/7846269
