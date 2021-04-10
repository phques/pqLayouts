gawk -F '' '$1 ~ /[qzwsxdcrfvtgbnjmklp]/ {print $0}' allDigraphs.txt | gawk -F '' '$2 ~ /[qzwsxdcrfvtgbnjmklp]/ {print $0}'
