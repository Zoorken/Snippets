import csv
import os
import statistics
from locale import *


def columnInt(matrix, i):
    #print(matrix);
    if (i == 9):
        temp = []
        for row in matrix:
            temp.append(float(row[9].replace(',','')))
        return temp
    else:
        return [int(row[i]) for row in matrix]


def findMinValue(dataColumn):
    minValue = 1000000000
    for nr in dataColumn:
        if nr < minValue:
            minValue = nr

    return minValue


def findMaxValue(dataColumn):
    maxValue = -100000000
    for nr in dataColumn:
        if nr > maxValue:
            maxValue = nr

    return maxValue


def main():

    __location__ = os.path.realpath(
        os.path.join(os.getcwd(), os.path.dirname(__file__)))
    tempColumn = []
    data = []

    output = open("result", 'w')

    # ip1 ip2 tempSumIcmp tempSumUdp tempSumTcp icmp tcp udp bytes duration
    with open(os.path.join(__location__, "stats"), "r") as myFile:
        myFileReader = csv.reader(myFile, delimiter=" ")
        next(myFileReader)
        for row in myFileReader:
            # print(row)
            data.append(row)

    columns = ["ICMP", "UDP", "TCP"]
    k = 0
    for i in range(2, 5):
        tempColumn = columnInt(data, i)
        output.write("Most active host pair:" + columns[k] + "\n")
        output.write("------------------------\n")
        output.write(
            "Mean: {} bytes".format(int(statistics.mean(tempColumn))) + "\n")
        output.write(
            "Min Value: {} bytes".format(int(findMinValue(tempColumn))) + "\n")
        output.write("Max Value: {} bytes".format(findMaxValue(tempColumn)) + "\n")
        output.write(
            "Standard Deviation: {} bytes".format(int(statistics.stdev(tempColumn))) + "\n\n")
        k += 1

    text = [" ICMP", " TCP", " UDP", ""]
    k = 0
    output.write(
        "------------------------\nOverall stats during experiment:\n------------------------\n")
    for i in range(5, 9):
        tempColumn = columnInt(data, i)
        output.write(
            "Total" + text[k] + " : {} sent bytes".format(int(sum(tempColumn))) + "\n")
        output.write( "Mean" + text[k] + " : {} sent bytes".format(int(statistics.mean(tempColumn))) + "\n")
        output.write("Min" + text[k] + " : {} sent bytes".format(int(findMinValue(tempColumn))) + "\n")
        output.write("Max" + text[k] + " : {} sent bytes".format(findMaxValue(tempColumn)) + "\n")
        output.write("Standard Deviation" + text[k] + " : {} bytes".format(int(statistics.stdev(tempColumn))) + "\n\n")
        k += 1

    tempColumn = columnInt(data, 9)
    print(statistics.mean(tempColumn))
    output.write("Total time: {} seconds".format(int(sum(tempColumn))) + "\n")
    output.write("Mean time: {} seconds".format(int(statistics.mean(tempColumn))) + "\n")
    output.write("Min time: {} seconds".format(int(findMinValue(tempColumn))) + "\n")
    output.write("Max time: {} seconds".format(int(findMaxValue(tempColumn))) + "\n")
    output.write("Standard Deviation: {} seconds".format(int(statistics.stdev(tempColumn))) + "\n")
# Doing a special for the time column cause the dot causes
# problemas.

if __name__ == "__main__":
    main()


# kolumn 1 och 2 skippar vi.

# kolumn 3->6 är intressanta
# 7-10 tar vi bara medeln för

# 192.168.186.10 109.105.109.218 0 276 1199064 166 1846331 4983 1853764 56.4
# 192.168.186.30 109.105.109.218 0 0 1173284 166 1846331 4983 1853764 56.4
# 192.168.186.30 109.105.109.218 0 0 1173012 166 1846331 4983 1853764 56.4
# 192.168.186.30 109.105.109.218 0 0 1173476 166 1846331 4983
# 1853764 56.4
