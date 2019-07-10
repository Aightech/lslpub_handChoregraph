for f in *.PNG.png.png; do 
    mv -- "$f" "$(basename -- "$f" .PNG.png.png).png"
done
