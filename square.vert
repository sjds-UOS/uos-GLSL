#version 450  // 声明着色器的版本号

// in_Position 绑定到属性索引0
// in_Color    绑定到属性索引1
in vec2 in_Position;  // 输入属性：表示从顶点数据中传入的点的位置信息
in vec3 in_Color;     // 输入属性：表示从顶点数据中传入的点的颜色信息

out vec3 ex_Color;    // 输出属性：表示将该颜色值传递至下一个着色器程序

// 着色器主函数
// 将传入的点位置信息输出到顶点缓冲区
// 并将传入的点颜色信息传递给下一个着色器程序
void main(void) {

    // 对于使用平面，我们只有x和y坐标，将z设置为0，w设置为1
    gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0);

    // 直接将颜色传递下去，不做任何修改
    ex_Color = in_Color;
}  