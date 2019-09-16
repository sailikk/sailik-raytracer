#ifdef _WIN32
#  define _CRT_SECURE_NO_DEPRECATE
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <time.h>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Light.h"
#include "Source.h"
#include "Object.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Plane.h"

using namespace std;

struct RGBType {
    double r;
    double g;
    double b;
};

void saveBMP(const string filename, int w, int h, int dpi, RGBType* data){
    FILE* file;
    int totalSize = w * h;
    int stride = 4 * totalSize;
    int fileSize = 54 + stride;

    double factor = 39.375;
    int meter = static_cast<int>(factor);
    int pixelsPerMeter = dpi * meter;

    unsigned char bmpFileHeader[14] = {
        'B', 'M', 0,0,0,0, 0,0,0,0, static_cast<char>(54),0,0,0 
    };
    unsigned char bmpInfoHeader[40];

    memset(bmpInfoHeader, 0, sizeof(bmpInfoHeader));

    bmpInfoHeader[0] = static_cast<char>(40);
    bmpInfoHeader[12] = 1;
    bmpInfoHeader[14] = 24;

    //Header:
    // Signature, Filesize, Reserved1, Reserved2, File Offset to Pixel Array
    bmpFileHeader[2] = (unsigned char)(fileSize);
    bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
    bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
    bmpFileHeader[5] = (unsigned char)(fileSize >> 24);

    //Width of the bitmap
    bmpInfoHeader[4] = (unsigned char)(w);
    bmpInfoHeader[5] = (unsigned char)(w>>8);
    bmpInfoHeader[6] = (unsigned char)(w>>16);
    bmpInfoHeader[7] = (unsigned char)(w>>24);

    //Height of the bitmap
    bmpInfoHeader[8] = (unsigned char)(h);
    bmpInfoHeader[9] = (unsigned char)(h>>8);
    bmpInfoHeader[10] = (unsigned char)(h>>16);
    bmpInfoHeader[11] = (unsigned char)(h>>24);

    //Pixels per meter Density
    bmpInfoHeader[21] = (unsigned char)(pixelsPerMeter);
    bmpInfoHeader[22] = (unsigned char)(pixelsPerMeter>>8);
    bmpInfoHeader[23] = (unsigned char)(pixelsPerMeter>>16);
    bmpInfoHeader[24] = (unsigned char)(pixelsPerMeter>>24);

    bmpInfoHeader[25] = (unsigned char)(pixelsPerMeter);
    bmpInfoHeader[26] = (unsigned char)(pixelsPerMeter>>8);
    bmpInfoHeader[27] = (unsigned char)(pixelsPerMeter>>16);
    bmpInfoHeader[28] = (unsigned char)(pixelsPerMeter>>24);

    bmpInfoHeader[29] = (unsigned char)(pixelsPerMeter);
    bmpInfoHeader[30] = (unsigned char)(pixelsPerMeter>>8);
    bmpInfoHeader[31] = (unsigned char)(pixelsPerMeter>>16);
    bmpInfoHeader[32] = (unsigned char)(pixelsPerMeter>>24);

    file = fopen(filename.c_str(), "wb"); //wb := 'W'rite 'B'inary

    fwrite(bmpFileHeader, 1, 14, file);
    fwrite(bmpInfoHeader, 1, 40, file);

    for (int i =0; i<totalSize; i++){
        RGBType rgb = data[i];

        double red = (data[i].r)*255;
        double green = (data[i].g)*255;
        double blue = (data[i].b)*255;

        unsigned char color[3] = {
            static_cast<unsigned char>((int) floor(blue)), 
            static_cast<unsigned char>((int) floor(green)),
            static_cast<unsigned char>((int) floor(red))
        };

        fwrite(color, 1, 3, file);
    }

    fclose(file);
}

int winningObjectIndex(vector<double>object_intersections) {
    // return the index of the winning intersection
    int index_of_minimum_value;

    // previous unnecessary calculations
    if ((object_intersections.size()) == 0) {
        // if there are no intersections
        return -1;
    } else if (object_intersections.size() == 1) {
        if (object_intersections.at(0) > 0) {
            // if that intersection is greater than zero
            // then it's our index of minimum value
            return 0;
        } else {
            // otherwise the only intersection value is negative
            return -1;
        }
    } else {
        // otherwise there is more than one intersection
        // first we find the maximum value
        double max = 0;
        for (int i = 0; i < object_intersections.size(); i++) {
            if (max < object_intersections.at(i)) {
                max = object_intersections.at(i);
            }
        }

        // then starting from the maximum value,
        // find the minimum positive value
        if (max > 0) {
            // we only want positive intersections
            for (int i = 0; i < object_intersections.size(); i++) {
                if (object_intersections.at(i) > 0
                    && object_intersections.at(i) <= max) {
                    max = object_intersections.at(i);
                    index_of_minimum_value = i;
                }
            }

            return index_of_minimum_value;
        } else {
            // all the intersections were negative
            return -1;
        }
    }
}

Color getColorAt(
        Vect intersection_position,
        Vect intersecting_ray_direction,
        vector<Object*> scene_objects,
        int index_of_winning_object,
        vector<Source*> light_sources,
        double accuracy,
        double ambientlight
) {
    Color winning_object_color =
        scene_objects.at(index_of_winning_object)->getColor();
    Vect winning_object_normal =
        scene_objects.at(index_of_winning_object)->getNormalAt(
            intersection_position
        );

    if (winning_object_color.getColorSpecial() == 2) {
        // checkerboard pattern
        int square = (int)floor(intersection_position.getVectX())
                   + (int)floor(intersection_position.getVectZ());

        // wakannai
        if ((square % 2) == 0) {
            // black tile
            winning_object_color.setColorRed(0);
            winning_object_color.setColorGreen(1);
            winning_object_color.setColorBlue(1);
        } else {
            //white tile
            winning_object_color.setColorRed(1);
            winning_object_color.setColorGreen(0);
            winning_object_color.setColorBlue(1);
        }
    }

    Color final_color = winning_object_color.colorScalar(ambientlight);

    if (winning_object_color.getColorSpecial() > 0
            && winning_object_color.getColorSpecial() <= 1) {
        // reflect from objects with specular intensity
                    // special [0-1]
            double dot1 = winning_object_normal.dotProduct(
                intersecting_ray_direction.negative()
            );

            // wakannai from here
            Vect scalar1 = winning_object_normal.vectMult(dot1);
            Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
            Vect scalar2 = add1.vectMult(2);
            Vect add2 =
                intersecting_ray_direction.negative().vectAdd(scalar2);
            Vect reflection_direction = add2.normalize();

            Ray reflection_ray (intersection_position, reflection_direction);

            // determine what the ray intersects with first
            vector<double> reflection_intersections;

            for (int r = 0; r < scene_objects.size(); r++) {
                reflection_intersections.push_back(
                    scene_objects.at(r)->findIntersection(reflection_ray)
                );
            }

            int index_of_winning_object_with_reflection =
                winningObjectIndex(reflection_intersections);

            if (index_of_winning_object_with_reflection != -1) {
                // reflection ray missed everything else
                if (reflection_intersections.at(
                        index_of_winning_object_with_reflection
                    ) > accuracy) {
                    // determine the position and direction at the point of intersection
                    // with the reflection ray
                    // the ray only affects the color if it is reflects off something

                    Vect reflection_intersection_position =
                        intersection_position.vectAdd(
                            reflection_direction.vectMult(
                                reflection_intersections.at(
                                    index_of_winning_object_with_reflection
                                )
                            )
                        );
                    Vect reflection_intersection_ray_direction = reflection_direction;

                    Color reflection_intersection_color =
                        getColorAt(reflection_intersection_position,
                                   reflection_intersection_ray_direction,
                                   scene_objects,
                                   index_of_winning_object_with_reflection,
                                   light_sources,
                                   accuracy,
                                   ambientlight
                        );
                    final_color = final_color.ColorAdd(
                        reflection_intersection_color.colorScalar(
                            winning_object_color.getColorSpecial()
                        )
                    );
                }
            }
    }

    for (int i = 0; i < light_sources.size(); i++) {
        Vect light_direction = 
            light_sources.at(i)->getLightPosition().vectAdd(
                intersection_position.negative()
            ).normalize();
        float cosine_angle =
            winning_object_normal.dotProduct(light_direction);
        if (cosine_angle > 0) { // acute angle b/w light source and obj
            // test for shadows
            bool shadowed = false;

            // goes to light source from point of intersection
            // see http://mathworld.wolfram.com/VectorAddition.html
            Vect distance_to_light =
                light_sources.at(i)->getLightPosition().vectAdd(
                    intersection_position.negative()
                ).normalize(); // remove normalize for more realistic shadow
            float distance_to_light_magnitude =
                distance_to_light.magnitude();

            Ray shadow_ray (
                intersection_position, distance_to_light.normalize()
            );

            vector<double> secondary_intersections;
            for (int j = 0;
                j < scene_objects.size() && shadowed == false;
                j++) {
                secondary_intersections.push_back(
                    scene_objects.at(j)->findIntersection(shadow_ray)
                );
            }

            for (int c = 0; c < secondary_intersections.size(); c++) {
                if (secondary_intersections.at(c) > accuracy) {
                    // if (secondary_intersections.at(c) <=
                    //         distance_to_light_magnitude) { // seems a bit weird
                        // for objects in the air, might remove later
                    shadowed = true;
                    // }
                }
            }

            if (shadowed == false) {
                final_color = final_color.ColorAdd(
                    winning_object_color.ColorMultiply(
                        light_sources.at(i)->getColor()
                    ).colorScalar(cosine_angle)); // by cos? wakannai
                if (winning_object_color.getColorSpecial() > 0 &&
                        winning_object_color.getColorSpecial() <= 1) {
                    // special [0-1]
                    double dot1 = winning_object_normal.dotProduct(
                        intersecting_ray_direction.negative()
                    );

                    // wakannai from here
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 =
                        intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();

                    double specular =
                        reflection_direction.dotProduct(light_direction);
                    if (specular > 0) {
                        specular = pow(specular, 10);
                        final_color = final_color.ColorAdd(
                            light_sources.at(i)->getColor().colorScalar(
                                specular*winning_object_color.getColorSpecial()
                            )
                        );
                    }
                    // to here
                }
            }
        }
    }
    return final_color.clip();
}

// int thisone;z
vector<Object*> scene_objects;

void makeCube (Vect corner1, Vect corner2, Color color) {
    // Triangle scene_tri (Vect(3, 0, 0), Vect(0, 3, 0), Vect(0, 0, 3), Color(0,0,0,0));
    // scene_objects.push_back(dynamic_cast<Object*>(&scene_tri));

    // corner 1
    double c1x = corner1.getVectX();
    double c1y = corner1.getVectY();
    double c1z = corner1.getVectZ();
    // // corner 2
    double c2x = corner2.getVectX();
    double c2y = corner2.getVectY();
    double c2z = corner2.getVectZ();

    // wakannai the algebra
    Vect A (c2x, c1y, c1z);
    Vect B (c2x, c1y, c2z);
    Vect C (c1x, c1y, c2z);

    Vect D (c2x, c2y, c1z);
    Vect E (c1x, c2y, c1z);
    Vect F (c1x, c2y, c2z);

    // left side
    scene_objects.push_back(new Triangle (D, A, corner1, color));
    scene_objects.push_back(new Triangle (corner1, E, D, color));
    // far side
    scene_objects.push_back(new Triangle (corner2, B, A, color));
    scene_objects.push_back(new Triangle (A, D, corner2, color));
    // right side
    scene_objects.push_back(new Triangle (F, C, B, color));
    scene_objects.push_back(new Triangle (B, corner2, F, color));
    // front side
    scene_objects.push_back(new Triangle (E, corner1, C, color));
    scene_objects.push_back(new Triangle (C, F, E, color));
    // top
    scene_objects.push_back(new Triangle (D, E, F, color));
    scene_objects.push_back(new Triangle (F, corner2, D, color));
    // bottom
    scene_objects.push_back(new Triangle (corner1, A, B, color));
    scene_objects.push_back(new Triangle (B, C, corner1, color));
}

int main(int argc, char *argv[]){
    cout << "rendering ..." << endl;

    clock_t t1, t2;
    t1 = clock();

    int dpi = 72;
    int width = 640;
    int height = 480;
    int n = width * height;
    RGBType *pixels = new RGBType[n];

    int aadepth = 4;
    double aathreshold = 0.1;
    double aspectratio = (double)width / (double)height;
    double ambientlight = 0.2;
    double accuracy = 0.000001;

    Vect O (0, 0, 0);
    Vect X (1, 0, 0);
    Vect Y (0, 1, 0);
    Vect Z (0, 0, 1);
    Vect new_sphere_loc (1.75, 0, 0);

    Vect campos (10, 10, 10);
    // X is into/outof screen
    // Y is up or down
    // Z is left or right...

    Vect look_at (0, 0, 0);
    Vect diff_btw (
        campos.getVectX() - look_at.getVectX(),
        campos.getVectY() - look_at.getVectY(),
        campos.getVectZ() - look_at.getVectZ()
    );
    Vect camdir = diff_btw.negative().normalize();
    Vect camright = Y.crossProduct(camdir);
    Vect camdown = camright.crossProduct(camdir);
    Camera scene_cam (campos, camdir, camright, camdown);

    Color white_light (1.0, 1.0, 1.0, 0);
    Color shiny_white (1.0, 1.0, 1.0, 0.4);
    Color pretty_green (0.5, 1.0, 0.5, 0.3);
    Color maroon (0.5, 0.25, 0.25, 0);
    Color bw_tiles (0, 0, 0, 2);
    Color gray (0.5, 0.5, 0.5, 0);
    Color black (0.0, 0.0, 0.0, 0);
    Color orange (0.5, 0.5, 0, 0);

    // light source(s)
    Vect light_position (-7, 10, -10);
    Light scene_light (light_position, white_light);
    vector<Source*> light_sources;
    light_sources.push_back(dynamic_cast<Source*>(&scene_light));

    // scene objects
    // Sphere scene_sphere (O, 1, pretty_green);
    Sphere scene_sphere2 (new_sphere_loc, 0.5, pretty_green);
    Plane scene_plane (Y, -1, bw_tiles);
    // Triangle scene_tri (Vect(3, 0, 0), Vect(0, 3, 0), Vect(0, 0, 3), orange);
    Sphere scene_sphere (Vect(-3, 0, 0), 3, shiny_white);

    makeCube(Vect (1, 1, 1), Vect(-1, -1, -1), orange);

    scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere));
    scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
    scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));
    // scene_objects.push_back(dynamic_cast<Object*>(&scene_tri));

    int thisone, aai;
    double xamnt, yamnt;

    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            thisone = y*width + x;

            // start with a blank pixel
            double tempRed[aadepth*aadepth];
            double tempGreen[aadepth*aadepth];
            double tempBlue[aadepth*aadepth];

            for (int aax = 0; aax < aadepth; aax++) {
                for (int aay = 0; aay < aadepth; aay++) {
                    aai = aay * aadepth + aax;
                    srand(time(0));

                    // create the ray from the camera to this pixel
                    if (aadepth == 1) {
                        // start with no anti-aliasing
                        if (width > height) {
                            // the image is wider than it is tall
                            xamnt = ((x + 0.5) / width) * aspectratio
                                    - (((width-height) / (double)height) / 2);
                            yamnt = ((height - y) + 0.5) / height;
                        } else if (height > width) {
                            // the image is taller than it is wide
                            xamnt = (x + 0.5) / width;
                            yamnt = (((height - y) + 0.5) / height) / aspectratio
                                    - (((height - width) / (double)width) / 2);
                                    // - (((height - width)/(double)width)/2); wakannai
                        } else {
                            // the image is square
                            xamnt = (x + 0.5) / width;
                            yamnt = ((height - y) + 0.5) / height;
                        }
                    } else {
                        // anti-aliasing
                        // int aa_offset = ;
                        if (width > height) {
                            // the image is wider than it is tall
                            xamnt = ((x + (double)aax/((double)aadepth - 1)) / width) * aspectratio
                                    - (((width-height) / (double)height) / 2);
                            yamnt = ((height - y) + (double)aax/((double)aadepth - 1)) / height;
                        } else if (height > width) {
                            // the image is taller than it is wide
                            xamnt = (x + (double)aax/((double)aadepth - 1)) / width;
                            yamnt = (((height - y) + (double)aax/((double)aadepth - 1)) / height) / aspectratio
                                    - (((height - width) / (double)width) / 2);
                                    // - (((height - width)/(double)width)/2); wakannai
                        } else {
                            // the image is square
                            xamnt = (x + (double)aax/((double)aadepth - 1)) / width;
                            yamnt = ((height - y) + (double)aax/((double)aadepth - 1)) / height;
                        }
                    }

                    Vect cam_ray_origin = scene_cam.getCameraPosition();
                    Vect cam_ray_direction = camdir.vectAdd(
                        camright.vectMult(xamnt - 0.5).vectAdd(
                            camdown.vectMult(yamnt - 0.5)
                        )
                    ).normalize();
                    Ray cam_ray (cam_ray_origin, cam_ray_direction);

                    vector<double> intersections;

                    for (int i = 0; i < scene_objects.size(); i++) {
                        intersections.push_back(
                            scene_objects.at(i)->findIntersection(cam_ray)
                        );
                    }

                    int index_of_winning_object = winningObjectIndex(intersections);

                    if (index_of_winning_object == -1){
                        // set the background black
                        tempRed[aai] = 0;
                        tempGreen[aai] = 0;
                        tempBlue[aai] = 0;
                    } else {
                        if (intersections.at(index_of_winning_object) > accuracy) {
                            // wakannai accuracy usage
                            Vect intersection_position = cam_ray_origin.vectAdd(
                                cam_ray_direction.vectMult(
                                    intersections.at(index_of_winning_object)
                                )
                            );
                            Vect intersecting_ray_direction = cam_ray_direction;
                            Color intersection_color = getColorAt(
                                intersection_position,
                                intersecting_ray_direction,
                                scene_objects,
                                index_of_winning_object,
                                light_sources,
                                accuracy,
                                ambientlight
                            );
                            tempRed[aai] = intersection_color.getColorRed();
                            tempGreen[aai] = intersection_color.getColorGreen();
                            tempBlue[aai] = intersection_color.getColorBlue();
                        }
                        // index corresponds to an object in our scene
                    }
                }
            }
            // average the pixel color
            double totalRed = 0;
            double totalGreen = 0;
            double totalBlue = 0;

            for (int iRed = 0; iRed < aadepth*aadepth; iRed++) {
                totalRed = totalRed + tempRed[iRed];
            }
            for (int iGreen = 0; iGreen < aadepth*aadepth; iGreen++) {
                totalGreen = totalGreen + tempGreen[iGreen];
                // totalGreen = 2*totalGreen + tempGreen[iGreen];
                // the above gives some drugs at least at aadepth = 2
            }
            for (int iBlue = 0; iBlue < aadepth*aadepth; iBlue++) {
                totalBlue = totalBlue + tempBlue[iBlue];
            }

            double avgRed = totalRed / pow(aadepth, 2);
            double avgGreen = totalGreen / pow(aadepth, 2);
            double avgBlue = totalBlue / pow(aadepth, 2);

            pixels[thisone].r = avgRed;
            pixels[thisone].g = avgGreen;
            pixels[thisone].b = avgBlue;
        }
    }

    saveBMP("scene_anti-aliased.bmp", width, height, dpi, pixels);

    delete pixels;
    // delete tempRed;
    // delete tempGreen;
    // delete tempBlue;

    t2 = clock();
    float diff = ((float)t2 - (float)t1) / CLOCKS_PER_SEC;

    cout << diff << " seconds" << endl;
    return 0;
}
