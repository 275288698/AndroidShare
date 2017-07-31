
public class Shader {

	public static int loadShader(int type, String shaderCode){

	    // 创建一个vertex shader类型(GLES20.GL_VERTEX_SHADER)
	    // 或fragment shader类型(GLES20.GL_FRAGMENT_SHADER)
	    int shader = GLES20.glCreateShader(type);

	    // 将源码添加到shader并编译之
	    GLES20.glShaderSource(shader, shaderCode);
	    GLES20.glCompileShader(shader);

	    return shader;
	}
	
	
	public Triangle() {
	    int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
	    int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);
	    mProgram = GLES20.glCreateProgram();             // 创建一个空的OpenGL ES Program
	    GLES20.glAttachShader(mProgram, vertexShader);   // 将vertex shader添加到program
	    GLES20.glAttachShader(mProgram, fragmentShader); // 将fragment shader添加到program
	    GLES20.glLinkProgram(mProgram);                  // 创建可执行的 OpenGL ES program
	}
	
	ublic void draw() {
	    // 将program加入OpenGL ES环境中
	    GLES20.glUseProgram(mProgram);
	    // 获取指向vertex shader的成员vPosition的 handle
	    mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");
	    // 启用一个指向三角形的顶点数组的handle
	    GLES20.glEnableVertexAttribArray(mPositionHandle);
	    // 准备三角形的坐标数据
	    GLES20.glVertexAttribPointer(mPositionHandle, COORDS_PER_VERTEX,
	                                 GLES20.GL_FLOAT, false,
	                                 vertexStride, vertexBuffer);
	    // 获取指向fragment shader的成员vColor的handle 
	    mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");
	    // 设置三角形的颜色
	    GLES20.glUniform4fv(mColorHandle, 1, color, 0);
	    // 画三角形
	    GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, vertexCount);
	    // 禁用指向三角形的顶点数组
	    GLES20.glDisableVertexAttribArray(mPositionHandle);
	}
	
}
