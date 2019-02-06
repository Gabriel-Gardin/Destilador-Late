import sys
from PyQt4 import QtGui, uic
from destilador_logic import destilador
import pyqtgraph as pg

pg.setConfigOption('background', 'w')

class MyWindow(QtGui.QMainWindow):
    def __init__(self):
        super(MyWindow, self).__init__()
        uic.loadUi('destilador.ui', self)
        self.dest = destilador(self.graphicsView, obj=self)# Instancia a biblioteca do destilador como dest.
        self.Bt_run.clicked.connect(self.btRun)
        self.Bt_stop.clicked.connect(self.btStop)
        self.actionSave_Ass.triggered.connect(self.save)
        self.Text_status.setText("Desligado")
        self.show()

    def btRun(self):
        #self.Text_status.setText("Destilando")
        self.dest.run(flag=True)

    def btStop(self):
        #self.Text_status.
        self.Text_status.setText("Desligado")
        self.dest.flag = False
        self.dest.stop()

    def save(self):
        self.dest.SaveAss()


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    window = MyWindow()
    sys.exit(app.exec_())

