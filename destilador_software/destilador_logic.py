import serial
import time
import csv
from PyQt4 import QtGui, QtCore
from PyQt4.QtGui import QFileDialog


class destilador(QtGui.QMainWindow):
    def __init__(self, graphics):
        QFileDialog.__init__(self)
        self.graphics = graphics
        self.X_data = []
        self.Y_data = []
        self.serial = serial.Serial('/dev/ttyACM0', 9600, timeout=1)

    def run(self, flag):
        complemento = 36.00
        complemento = str(complemento)
        self.flag = flag
        self.serial.write("#pwr" + complemento + "\n")
        time.sleep(0.1)
        self.serial.write("#pwr" + complemento + "\n")
        time.sleep(0.1)

        while(self.flag == True):
            self.serial.flushInput()
            self.serial.write("#data"+complemento+"\n")
            time.sleep(0.1)
            self.serial.write("#data"+complemento+"\n")
            time.sleep(0.1)

            while(self.serial.in_waiting>0):
                line = self.serial.readline().strip()
                plot_data = line.split(";")
                print(len(plot_data))
                if(len(plot_data)):
                    print(line)
                    self.X_data.append(float(plot_data[1]))
                    self.Y_data.append(float(plot_data[2]))
                    self.fileDialog = QtGui.QFileDialog(self)
                    #print(self.X_data, self.Y_data)
                    self.graphics.plot(self.X_data, self.Y_data)
                elif(line == '#'):
                    print("haha!")

            QtCore.QCoreApplication.processEvents()
            time.sleep(1)


    def stop(self):
        print ('stop')


    def SaveAss(self):
        print("save")
        name = QtGui.QFileDialog.getSaveFileName(self, 'Save File')

        with open(name+'.csv', 'wb') as destilador_data_file:
            writer = csv.writer(destilador_data_file, delimiter=',')
            for i in range(len(self.X_data)):
                writer.writerow([self.X_data[i], self.Y_data[i]])




