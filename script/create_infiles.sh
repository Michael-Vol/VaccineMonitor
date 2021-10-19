#! /bin/bash

#check for the required number of arguments
rm -R -- */

inputFile=$1;
inputDir=$2;
numFilesPerDirectory=$3;
if [ $# -ne 3 ]; then
    echo "Please provide the required number of arguments."
    exit 1
elif [ ! -s $inputFile ]; then
    echo "Input file is empty or doesn't exist."
    exit 1
elif [ -d $inputDir ]; then
    echo "Input directory already exists."
    exit 1
elif [[ ! $numFilesPerDirectory =~ ^-?[0-9]+$ ]]; then 
    echo "Number of Files per Directory is not a number."
    exit 1
elif  [ $numFilesPerDirectory -lt 0 ]; then
    echo "Number of Files per Directory is not a positive number."
    exit 1
fi

#create directory
mkdir -p $inputDir

declare -a tokens;
declare -a countries;

cd $inputDir 
while IFS= read -r line;
do
    IFS=' ' read -r -a tokens <<< $line
    country=${tokens[3]}
    if [ ! -d $country ]; then 
        echo "Creating directory of: $country"
        countries+=("$country")
        mkdir -p $country
        cd $country
        for((i = 0; i < $numFilesPerDirectory; i++)); do
            touch "${country}-$((${i}+1)).txt"
        done
        cd ../
    fi

done < $"../$inputFile"

for country in "${countries[@]}";
do
    echo "Distributing records of $country"
    selector=0
    cd $country
    while IFS= read -r line;
    do  
        IFS=' ' read -r -a tokens <<< $line
        fileCountry=${tokens[3]}
        if [ $country == $fileCountry ]; then
            if [ $selector -eq $numFilesPerDirectory ]; then
                selector=0
            fi 
                selector=$((selector+1))
                echo $line >> "${country}-${selector}.txt"
        fi
    done < $"../../$inputFile"
    cd ../
done



