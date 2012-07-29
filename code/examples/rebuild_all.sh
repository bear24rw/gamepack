for dir in `find ./ -type d`
do
    echo "----------------------------------"
    echo $dir
    echo "----------------------------------"
    (cd $dir && make clean && make)
done
