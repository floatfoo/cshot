FROM archlinux
RUN pacman -Sy --noconfirm libpng
RUN pacman -Sy --noconfirm xorg-server
RUN pacman -Sy --noconfirm make
RUN pacman -Sy --noconfirm gcc
CMD ls /lib64/ | grep X11
COPY . .
RUN make -k

