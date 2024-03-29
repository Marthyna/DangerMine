#version 330 core

in vec4 position_world;
in vec4 normal;
in vec4 position_model;
in vec2 texcoords;
in vec3 gouraud_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define GUN 0
#define AIM 1
#define PLANE 2
#define LANDSCAPE 3
#define SKY 4
#define BULLET 5
#define ROCK 6
#define PICKAXE 7
#define ENEMY 8
uniform int object_id;

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D sand;
uniform sampler2D metal;
uniform sampler2D landscape;
uniform sampler2D sky;
uniform sampler2D rock;

out vec3 color;

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(-1.0,1.0,0.0,0.0));
    vec4 l_dir = normalize(l - camera_position);

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;
    vec3 Kd0;
    vec3 Ka;
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    float q; // Expoente especular para o modelo de iluminação de Phong
    vec4 r = -l + 2*n*(dot(n,l)); // vetor de reflexão especular ideal

    if (object_id == AIM)
    {
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        float raio = 1.0f;

        vec4 p = bbox_center + raio * normalize(position_model - bbox_center);
        vec4 p_dist = p - bbox_center;

        float theta = atan(p_dist.x, p_dist.z);
        float phi = asin(p_dist.y/raio);

        U = (theta + M_PI)/(2*M_PI);
        V = (phi + M_PI_2)/(M_PI);

        Kd0 = texture(metal, vec2(U,V)).rgb;
        float lambert = max(0,dot(n,l));
        color = Kd0 * (lambert + 0.01);
        color = pow(color, vec3(1.0,1.0,1.0)/2.2);
    } 
    if (object_id == BULLET)
    {
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        float raio = 1.0f;

        vec4 p = bbox_center + raio * normalize(position_model - bbox_center);
        vec4 p_dist = p - bbox_center;

        float theta = atan(p_dist.x, p_dist.z);
        float phi = asin(p_dist.y/raio);

        U = (theta + M_PI)/(2*M_PI);
        V = (phi + M_PI_2)/(M_PI);

        Kd0 = texture(metal, vec2(U,V)).rgb;
        float lambert = max(0,dot(n,l));
        color = Kd0 * (lambert + 0.01);
        color = pow(color, vec3(1.0,1.0,1.0)/2.2);
    } 
    else if ( object_id == GUN || object_id == PICKAXE)
        color = gouraud_color;

    else if ( object_id == PLANE )
    {
        U = texcoords.x;
        V = texcoords.y;

        Kd0 = texture(sand, vec2(U,V)).rgb;
        float lambert = max(0,dot(n,l));
        color = Kd0 * (lambert + 0.01);
        color = pow(color, vec3(1.0,1.0,1.0)/2.2);
    }
    else if ( object_id == LANDSCAPE )
    {
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - bbox_min.x)/(bbox_max.x - bbox_min.x);
        V = (position_model.y - bbox_min.y)/(bbox_max.y - bbox_min.y);

        Kd0 = texture(landscape, vec2(U,V)).rgb;
        color = pow(Kd0, vec3(1.0,1.0,1.0)/2.2);
    }
    else if ( object_id == SKY )
    {
        U = texcoords.x;
        V = texcoords.y;

        Kd0 = texture(sky, vec2(U,V)).rgb;
        color = pow(Kd0, vec3(1.0,1.0,1.0)/2.2);
    }
    else if ( object_id == ROCK )
    {
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - bbox_min.x)/(bbox_max.x - bbox_min.x);
        V = (position_model.y - bbox_min.y)/(bbox_max.y - bbox_min.y);

        Kd0 = texture(rock, vec2(U,V)).rgb;
        float lambert = max(0,dot(n,l));
        color = Kd0 * (lambert + 0.01);
        color = pow(color, vec3(1.0,1.0,1.0)/2.2);
    }
    else if ( object_id == ENEMY )
    {
        Kd = vec3(0.2667, 0.902, 0.0902);
        Ks = vec3(0.8,0.8,0.8);
        Ka = vec3(0.0,0.0,0.0);
        q = 42.0;
       
        vec3 I = vec3(0.0, 0.0, 0.0);

        if (dot(normalize(p - l), normalize(l_dir)) < 0.86602) {
            I = vec3(1.0,1.0,1.0);
        }

        vec3 Ia = vec3(0.2, 0.2, 0.2);
        vec3 lambert_diffuse_term = Kd*I*max(0, dot(n,l));
        vec3 ambient_term = Ka*Ia;
        vec3 phong_specular_term  = Ks*I*pow(max(0, dot(r,v)), q); 
        color = lambert_diffuse_term + ambient_term + phong_specular_term;
        color = pow(color, vec3(1.0,1.0,1.0)/2.2);         
    }
}

