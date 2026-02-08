#!/bin/bash
set -e

build_dir="build"
obj_dir="$build_dir/obj"
iso_dir="$build_dir/isodir"
boot_dir="boot"
kernel_dir="kernel"

echo -e "Building..."
mkdir -p $obj_dir
mkdir -p $iso_dir/boot/grub

echo -e "[1/3] Assembling boot"
nasm -f elf32 $boot_dir/boot.s -o $obj_dir/boot.o
nasm -f elf32 kernel/gdt/flush.s -o $obj_dir/gdt_flush.o
nasm -f elf32 kernel/idt/isr.s -o $obj_dir/isr.o
nasm -f elf32 kernel/idt/load.s -o $obj_dir/idt_load.o

echo -e "[2/3] Compiling kernel"
obj_files="$obj_dir/boot.o"

while IFS= read -r -d '' c_file; do
    rel_path="${c_file#$kernel_dir/}"
    obj_subdir="$obj_dir/$(dirname "$rel_path")"
    mkdir -p "$obj_subdir"
    filename=$(basename "$c_file" .c)
    obj_file="$obj_subdir/$filename.o"
    echo "  Compiling $c_file..."
    gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -O2 -Wall -Wextra -I lib/include -I kernel \
        -c "$c_file" -o "$obj_file"
    obj_files="$obj_files $obj_file"
done < <(find $kernel_dir lib -name "*.c" -print0 2>/dev/null)

echo -e "[3/3] Linking and creating ISO"
gcc -m32 -T linker.ld -ffreestanding -O2 -nostdlib \
    $obj_files $obj_dir/gdt_flush.o $obj_dir/isr.o $obj_dir/idt_load.o -o $iso_dir/boot/xf-os.bin -lgcc

cp $boot_dir/grub.cfg $iso_dir/boot/grub/

grub-mkrescue -o $build_dir/xf-os.iso $iso_dir 2>/dev/null

echo -e "Build complete."
qemu-system-i386 -cdrom $build_dir/xf-os.iso