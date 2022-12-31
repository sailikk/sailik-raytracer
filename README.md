# raytracer
This is a raytracer I made by learning from various sources online.

I was actually very pressed for time when making this project, so the code is not as clean as I would like it to be. Many optimizations can be made, including:

* Instead of using a Vector class, I can use vector structure built into C++. This should speed things up a lot.
* Splitting up long functions
* Shortening the names of variables (I used more descriptive ones to start with, since it was very confusing at first)
* Having some sort of input taking mechanism to set parameters if user wants to (right now you must do this manually in the code, which might be easy for me but not so much for others...)
* Taking input from files apart from UI as well, perhaps some preconfigured scenes.
* Adding full documentation
* Adding many more ray-tracing functionalities long term, like transparent objects, directional lights, texture mapping, etc.

Looks sort of like an igloo, but with a floating sphere that is green.
![Looks like an Igloo](https://i.imgur.com/zI5Xj9O.png)

Same image but anti-alised with depth 4.
![Imgur](https://i.imgur.com/rMt7GZV.png)

Some out-of-gamut colours. My friend thinks it's cool.
![Imgur](https://i.imgur.com/ZaRgTlh.png)

How do you run this?
```
make
./PhysicsRayTracer
```
Now a bitmap named `scene.bmp` should be present in the directory. Do `xdg-open scene.bmp` if you're on Linux or `cmd.exe /C start scene.bmp` if you are on WSL.

You should now be seeing your image. I completed this project on WSL Bash.

How can you change the image? Find the word "CUSTOMIZABLE" in `main.cpp`. You can customize the corresponding values to get a different image.

Helpful sources:
* http://www.povray.org/
* [Ray Tracing from the Ground Up by Kevin Suffern](https://www.amazon.ca/Ray-Tracing-Ground-Kevin-Suffern/dp/1568812728)
* [Video series by Caleb](https://www.youtube.com/watch?v=k_aRiYSXcyo&list=PLHm_I0tE5kKPPWXkTTtOn8fkcwEGZNETh)
