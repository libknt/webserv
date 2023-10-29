
#!/bin/bash

make -C ../
cp ../server ./
error_occurred=false

for i in $(seq 1 23); do
    output=$(./server test${i}.conf 2>&1 > /dev/null)

    if [[ -z "$output" ]]; then
        echo "test${i}.conf: エラーです"
        error_occurred=true
    fi
done

if $error_occurred; then
    echo "何らかの設定ファイルでエラーが発生しました。"
fi

rm -f server