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

# Truncate function
trunc <- function(x, ..., prec = 0) base::trunc(x * 10^prec, ...) / 10^prec;

# Read 'CSV'
data <- read.csv("csv_times", header=TRUE, sep=",")
names <- c("cat", "kat", "bat")

#PNG Device
png(
	filename="bench.png",
	width = 580, height = 450,
	units = "px", pointsize = 15
)

# Margins
par(mar = c(5.1, 4.1, 2.5, 2.1))

# Plot Graphs
ylim <- c(0, 1.20*max(data$times))

xx = barplot(data$times, ylim=ylim, lwd=3, space=0.5, axes=FALSE)

numbers <- data$times
numbers <- paste(numbers, " s", sep="")

slower1 <- "Slower than cat:"
slower1 <- rep(slower1, times=3)
factor  <- data$times/data$times[1]
slower1 <- paste(slower1, trunc(factor, prec=2))
slower1 <- paste(slower1, "x", sep="")
slower1[1] <- "Reference value"

slower2 <- "Slower than kat:"
slower2 <- rep(slower2, times=3)
factor  <- data$times/data$times[2]
slower2 <- paste(slower2, trunc(factor, prec=2))
slower2 <- paste(slower2, "x", sep="")
slower2[1] <- ""

#GRID
grid(NULL, NULL)

# Redraw again, to put graph above grid
par(new=TRUE)
xx = barplot(
		data$times, names.arg=names,
		xlab="Tools", ylab="Time (Seconds)",
		main="Time to read/highlight sqlite3.c (4MiB - 133 klocs)",
		col="#1d995b", border="black",
		ylim=ylim, lwd=3, space=0.5
	)

# Complimentary texts
text(x = xx, y = data$times+0.75, label = numbers, pos = 3, cex = 0.8, col = "blue")
text(x = xx, y = data$times+0.50, label = slower1, pos = 3, cex = 0.8, col = "red")
text(x = xx, y = data$times+0.25, label = slower2, pos = 3, cex = 0.8, col = "red")

#Device off
dev.off()
