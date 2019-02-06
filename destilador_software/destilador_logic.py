import serial
import serial.tools.list_ports
import time
import csv
from PyQt4 import QtGui, QtCore
from PyQt4.QtGui import QFileDialog


class destilador(QtGui.QMainWindow):
    def __init__(self, obj):
        QFileDialog.__init__(self)
        self.fileDialog = QtGui.QFileDialog(self)
        self.text_status = obj.Text_status
        self.text_status_2 = obj.Text_status_2
        self.X_data = []
        self.Y_data = []
        self.graphicsView = obj.graphicsView
        self.serial_check = False

    def run(self, flag):
        ports = list(serial.tools.list_ports.comports())
        if(ports):
            for p in ports:
                if(p.serial_number == '8543836303935191F060'):
                    print p.device
                    self.serial = serial.Serial(p.device, 9600, timeout=1)
                    self.serial_check = True
                else:
                    self.text_status.setText("Destilador nao conectado")
        else:
            self.text_status.setText("Destilador nao conectado")


        if(self.serial_check):
            #print("nao chegou")
            self.text_status.setText("Destilando")
            complemento = 36.00
            complemento = str(complemento)
            self.serial.write("#on" + "\n")
            time.sleep(0.1)
            self.serial.write("#on" + "\n")
            time.sleep(0.1)
            self.flag = flag
            self.serial.write("#pwr" + complemento + "\n")
            time.sleep(0.1)
            self.serial.write("#pwr" + complemento + "\n")
            time.sleep(0.1)

            while(self.flag == True):
                self.serial.flushInput()
                self.serial.write("#temp"+"\n")
                time.sleep(0.15)
                self.serial.write("#temp"+"\n")
                time.sleep(0.15)
                QtCore.QCoreApplication.processEvents()

                while(self.serial.in_waiting>0):
                    line = self.serial.readline().strip()
                    plot_data = line.split(";")
                    print(plot_data)
                    if(len(plot_data == 1)):
                        self.text_status_2.setText(str(plot_data))
                    else:
                        pass
                    print(len(plot_data))
                    if(len(plot_data)==3):
                        print(len(plot_data))
                        self.X_data.append(float(plot_data[1]))
                        self.Y_data.append(float(plot_data[3]))
                        print(self.X_data, self.Y_data)
                        self.graphicsView.plot(self.X_data, self.Y_data)

                    time.sleep(1)
                    #print(line)



    def stop(self):
        if(self.flag == True):
            self.serial.write("#off" + "\n")
            time.sleep(0.1)
            self.serial.write("#off" + "\n")
            time.sleep(0.1)
            print ('stop')
        else:
            pass


    def SaveAss(self):
        print("save")
        name = QtGui.QFileDialog.getSaveFileName(self, 'Save File')

        with open(name+'.csv', 'wb') as destilador_data_file:
            writer = csv.writer(destilador_data_file, delimiter=',')
            for i in range(len(self.X_data)):
                writer.writerow([self.X_data[i], self.Y_data[i]])




