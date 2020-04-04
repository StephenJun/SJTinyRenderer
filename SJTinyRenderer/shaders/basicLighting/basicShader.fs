#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos;  //光源位置
uniform vec3 viewPos;  //相机位置
uniform vec3 lightColor; //光源颜色
uniform vec3 objectColor; //物体本身颜色

//基本冯式光照模型
void main()
{
    // ambient环境光根据光源颜色*环境光系数可以得到
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 漫反射光照颜色根据法线方向和光源方向的夹角得到漫反射系数*光源颜色得到
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular 镜面光照颜色根据视角方向和光源方向在该片元的反射方向的夹角得到
    float specularStrength =1;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
      
    //三个部分的光照颜色叠加 * 物体自身颜色可以得到最终的输出颜色
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 