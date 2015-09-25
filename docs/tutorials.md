Tutorials
=========

SFML
----

https://github.com/LaurentGomila/SFML/wiki/Tutorials

### Compiling SFML on *NIX
( see: http://www.mjbshaw.com/2013/02/building-sfml-2-with-c11-on-os-x.html )

~~~{.sh}
wget http://www.sfml-dev.org/files/SFML-2.2-doc.zip
unzip SFML-2.2-doc.zip
cd SFML-2.2
mkdir build
cd build

# for latest version of OSX with clang
cmake -G 'Unix Makefiles' \
      -DCMAKE_OSX_ARCHITECTURES='i386;x86_64' \
      -DCMAKE_CXX_COMPILER='/usr/bin/clang++' \
      -DCMAKE_C_COMPILER='/usr/bin/clang' \
      -DSFML_BUILD_FRAMEWORKS='ON' \
      -DSFML_INSTALL_XCODE4_TEMPLATES='ON' \
      -DCMAKE_CXX_FLAGS='-stdlib=libc++' \
      -DCMAKE_C_FLAGS='-stdlib=libc++' ../

cd build
make -j4
sudo make install
~~~
