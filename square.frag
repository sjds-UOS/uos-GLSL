#version 450            // 声明着色器的版本号

precision highp float;  // 设置浮点数精度为 highp

in  vec3 ex_Color;      // 输入属性：表示从顶点着色器传入的颜色值
out vec4 gl_FragColor;  // 输出属性：表示输出到渲染缓冲区的颜色值

// 将顶点颜色值作为像素颜色值直接输出
void main(void) {
    gl_FragColor = vec4(ex_Color, 1.0);
}