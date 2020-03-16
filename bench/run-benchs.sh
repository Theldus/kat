#!/usr/bin/env bash

#
# MIT License
#
# Copyright (c) 2020 Davidson Francis <davidsondfgl@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
elapsed_cat=0
elapsed_kat=0
elapsed_bat=0
iterations=8

# Check for Kat
if [ ! -f "../kat" ];
then
	echo "Kat not found, please build it first!"
	exit 1
fi

# Check for R
if [ ! -x "$(command -v Rscript)" ]
then
	echo "Rscript not found! GDB and Rscript is necessary in order to"
	echo "run the benchmarks."
	exit 1
fi

# Check for Bat
if [ ! -x "$(command -v bat)" ]
then
	if [ ! -f "bat" ]
	then
		echo "Bat not found! downloading..."
		wget -q "https://github.com/sharkdp/bat/releases/download/v0.12.1/\
bat-v0.12.1-x86_64-unknown-linux-gnu.tar.gz"
		tar xf bat-v*.tar.gz
		mv bat-v*/bat .
		rm -rf ./bat-v*

		echo "Exporting Bat to PATH..."
		export PATH=$PATH:$(pwd)/
	else
		export PATH=$PATH:$(pwd)/
	fi
fi

# Check for sqlite3.c amalgamated.
if [ ! -f "sqlite3.c" ]
then
	echo "sqlite3.c not found, downloading...."
	wget -q "https://gist.github.com/Theldus/1c9b7d59b3da7ed02533871bc98b8eff\
/raw/2f27e4d391ef53078753a3dc3bfc54e0aeb8700a/sqlite3.c"
fi

# Prepare CSV
echo "times" > csv_times

echo -e "\nThis may take less than one minute, please be patient..."
for (( step=0; step<iterations; step++ ))
do
	echo    "Test #$step..."
	echo -n "  > Cat..."

	# Run Cat
	#
start_time="$(date -u +%s.%N)"
	cat sqlite3.c > /dev/null
end_time="$(date -u +%s.%N)"

	elapsed="$(bc <<<"$end_time-$start_time" | awk '{printf "%f", $0}')"
	elapsed_cat="$(bc <<<"$elapsed_cat+$elapsed")"
	echo "Time spent: $elapsed s"

	# Run Kat
	echo -n "  > Kat..."
start_time="$(date -u +%s.%N)"
	../kat sqlite3.c > /dev/null
end_time="$(date -u +%s.%N)"

	elapsed="$(bc <<<"$end_time-$start_time" | awk '{printf "%f", $0}')"
	elapsed_kat="$(bc <<<"$elapsed_kat+$elapsed")"
	echo "Time spent: $elapsed s"

	# Run Bat
	echo -n "  > Bat..."
start_time="$(date -u +%s.%N)"
	bat --color=always -p sqlite3.c > /dev/null
end_time="$(date -u +%s.%N)"

	elapsed="$(bc <<<"$end_time-$start_time" | awk '{printf "%f", $0}')"
	elapsed_bat="$(bc <<<"$elapsed_bat+$elapsed")"
	echo "Time spent: $elapsed s"

	size=$((size + increment))
done

elapsed_cat="$(bc <<<"scale=10; $elapsed_cat/$iterations")"
elapsed_kat="$(bc <<<"scale=10; $elapsed_kat/$iterations")"
elapsed_bat="$(bc <<<"scale=10; $elapsed_bat/$iterations")"

echo ""
echo "Elapsed cat mean $elapsed_cat"
echo "Elapsed kat mean $elapsed_kat"
echo "Elapsed bat mean $elapsed_bat"
echo "$elapsed_cat" >> csv_times
echo "$elapsed_kat" >> csv_times
echo "$elapsed_bat" >> csv_times

# Plot graph =)
echo "Plotting graph..."
Rscript plot.R &> /dev/null
