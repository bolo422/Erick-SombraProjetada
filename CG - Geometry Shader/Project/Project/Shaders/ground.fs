#version 330 core

struct Light
{
	vec3 position;
	vec3 specular;
    vec3 color;
};

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

// texture samplers
uniform vec3 cameraPos;
uniform sampler2D groundTexture;
uniform sampler2D objectTexture;
uniform Light light;

uniform vec3 rectTopLeft;
uniform vec3 rectTopRight;
uniform vec3 rectBottomLeft;
uniform vec3 rectBottomRight;

vec2 intersect(vec3 rayOrigin, vec3 rayEnd, vec3 TopLeft, vec3 TopRight, vec3 BottomLeft, vec3 BottomRight)
{
	vec3 u = BottomLeft - TopLeft;
	vec3 v = TopRight - TopLeft;
	vec3 n = cross(u, v);
	vec3 dir = rayEnd - rayOrigin;
	vec3 w0 = rayOrigin - TopLeft;
	float a = -dot(n, w0);
	float b = dot(n, dir);
	float r = a / b;

    if (r < 0.0)
    {
        return vec2(0, 0); 
    }    	

	vec3 ip = rayOrigin + r * dir;    
    return vec2(ip.x, ip.y);
}

void main()
{
    vec2 P;
    vec4 c;
    
    P = intersect(light.position, FragPos, rectTopLeft, rectTopRight, rectBottomLeft, rectBottomRight);
    c = texture(groundTexture, TexCoord);

  // ambient
  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * light.color;

  //diffuse 
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * light.color;
  
  // specular
  float specularStrength = 0.5;
  vec3 viewDir = normalize(cameraPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);  
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * light.color;  
    
  vec3 result = (ambient + diffuse + specular);

  if(P.x == 0.0f && P.y == 0.0f)
  {
      //FragColor = c;
      FragColor = vec4(result, 1.0) * texture(groundTexture, TexCoord);
  }
  else
  {
      FragColor = (vec4(result, 1.0) * texture(groundTexture, TexCoord)) * (1 - texture(objectTexture, vec2(P.x, 1-P.y)).a * 0.6);
  }  

}
