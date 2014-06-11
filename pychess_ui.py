#!/usr/bin/env python

from PyQt4 import QtCore, QtGui

class Ui_MainWindow(object):
	def setupUi(self, MainWindow):
		MainWindow.setObjectName("MainWindow")
		MainWindow.resize(800, 600)

		self.mainContainer = QtGui.QWidget(MainWindow)
		self.mainContainer.setObjectName("mainContainer")
		self.mainLayout = QtGui.QHBoxLayout(self.mainContainer)
		self.mainLayout.setObjectName("mainLayout")

		self.leftContainer = QtGui.QVBoxLayout()
		self.leftContainer.setSpacing(0)
		self.leftContainer.setMargin(10)
		self.leftContainer.setObjectName("leftContainer")

		self.rightContainer = QtGui.QVBoxLayout()
		self.rightContainer.setSpacing(0)
		self.rightContainer.setMargin(10)
		self.rightContainer.setObjectName("rightContainer")

		self.label = QtGui.QLabel(self.mainContainer)
		font = QtGui.QFont()
		font.setPointSize(14)
		font.setWeight(75)
		font.setBold(True)
		self.label.setFont(font)
		self.label.setAlignment(QtCore.Qt.AlignCenter)
		self.label.setObjectName("label")
		self.leftContainer.addWidget(self.label)

		self.label2 = QtGui.QLabel(self.mainContainer)
		font = QtGui.QFont()
		font.setPointSize(14)
		font.setWeight(75)
		font.setBold(True)
		self.label2.setFont(font)
		self.label2.setAlignment(QtCore.Qt.AlignCenter)
		self.label2.setObjectName("label2")
		self.rightContainer.addWidget(self.label2)

		sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Ignored, QtGui.QSizePolicy.Ignored)
		sizePolicy.setHorizontalStretch(1)
		sizePolicy.setVerticalStretch(1)
		sizePolicy.setHeightForWidth(True)

		self.whiteButtons = [None] * 6
		self.blackButtons = [None] * 6
		for i in range(0, 6):
			self.whiteButtons[i] = QtGui.QPushButton(self.mainContainer)
			self.blackButtons[i] = QtGui.QPushButton(self.mainContainer)
			self.whiteButtons[i].setSizePolicy(sizePolicy)
			self.blackButtons[i].setSizePolicy(sizePolicy)
			self.whiteButtons[i].setObjectName("whiteButton" + str(i))
			self.blackButtons[i].setObjectName("blackButton" + str(i))
			self.whiteButtons[i].setFlat(True)
			self.blackButtons[i].setFlat(True)
			self.whiteButtons[i].setIconSize(QtCore.QSize(64, 64))
			self.blackButtons[i].setIconSize(QtCore.QSize(64, 64))

	        	self.leftContainer.addWidget(self.whiteButtons[i])
	        	self.rightContainer.addWidget(self.blackButtons[i])

	        self.centerContainer = QtGui.QGridLayout()
	        self.centerContainer.setMargin(10)
	        self.centerContainer.setSpacing(0)
	        self.centerContainer.setObjectName("centerContainer")

		self.boardButtons = [None] * 64
		for i in range(0, 8):
			for j in range(0, 8):
				self.boardButtons[i * 8 + j] = QtGui.QPushButton(self.mainContainer)
				self.boardButtons[i * 8 + j].setSizePolicy(sizePolicy)
				self.boardButtons[i * 8 + j].setObjectName("boardButton" + str(i * 8 + j))
				self.boardButtons[i * 8 + j].setIconSize(QtCore.QSize(48, 48))
				self.centerContainer.addWidget(self.boardButtons[i * 8 + j], i, j, 1, 1)

		self.mainLayout.addLayout(self.leftContainer)
        	self.mainLayout.addLayout(self.centerContainer)
		self.mainLayout.addLayout(self.rightContainer)
        	MainWindow.setCentralWidget(self.mainContainer)

        	self.menubar = QtGui.QMenuBar(MainWindow)
        	self.menubar.setGeometry(QtCore.QRect(0, 0, 700, 23))
        	self.menubar.setObjectName("menubar")
        	self.menuFile = QtGui.QMenu(self.menubar)
        	self.menuFile.setObjectName("menuFile")
        	self.menuHistory = QtGui.QMenu(self.menubar)
        	self.menuHistory.setObjectName("menuHistory")
        	self.menuSettings = QtGui.QMenu(self.menubar)
        	self.menuSettings.setObjectName("menuSettings")
        	MainWindow.setMenuBar(self.menubar)
        	self.statusbar = QtGui.QStatusBar(MainWindow)
        	self.statusbar.setObjectName("statusbar")
        	MainWindow.setStatusBar(self.statusbar)
        	self.actionNew_Game_F2 = QtGui.QAction(MainWindow)
        	self.actionNew_Game_F2.setObjectName("actionNew_Game_F2")
        	self.actionClose_alt_F4 = QtGui.QAction(MainWindow)
        	self.actionClose_alt_F4.setObjectName("actionClose_alt_F4")
        	self.actionUndo_ctrl_z = QtGui.QAction(MainWindow)
        	self.actionUndo_ctrl_z.setObjectName("actionUndo_ctrl_z")
        	self.actionEdit_Settings = QtGui.QAction(MainWindow)
        	self.actionEdit_Settings.setObjectName("actionEdit_Settings")
        	self.menuFile.addAction(self.actionNew_Game_F2)
        	self.menuFile.addSeparator()
        	self.menuFile.addAction(self.actionClose_alt_F4)
        	self.menuHistory.addAction(self.actionUndo_ctrl_z)
        	self.menuSettings.addAction(self.actionEdit_Settings)
        	self.menubar.addAction(self.menuFile.menuAction())
        	self.menubar.addAction(self.menuHistory.menuAction())
        	self.menubar.addAction(self.menuSettings.menuAction())

		self.retranslateUi(MainWindow)
		QtCore.QMetaObject.connectSlotsByName(MainWindow)

	def retranslateUi(self, MainWindow):
		MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "qtGenChess", None, QtGui.QApplication.UnicodeUTF8))
		self.label.setText(QtGui.QApplication.translate("MainWindow", "WHITE", None, QtGui.QApplication.UnicodeUTF8))
		self.label2.setText(QtGui.QApplication.translate("MainWindow", "BLACK", None, QtGui.QApplication.UnicodeUTF8))
		self.menuFile.setTitle(QtGui.QApplication.translate("MainWindow", "File", None, QtGui.QApplication.UnicodeUTF8))
		self.menuHistory.setTitle(QtGui.QApplication.translate("MainWindow", "History", None, QtGui.QApplication.UnicodeUTF8))
		self.menuSettings.setTitle(QtGui.QApplication.translate("MainWindow", "Settings", None, QtGui.QApplication.UnicodeUTF8))
		self.actionNew_Game_F2.setText(QtGui.QApplication.translate("MainWindow", "New Game (F2)", None, QtGui.QApplication.UnicodeUTF8))
		self.actionClose_alt_F4.setText(QtGui.QApplication.translate("MainWindow", "Close (alt+F4)", None, QtGui.QApplication.UnicodeUTF8))
		self.actionUndo_ctrl_z.setText(QtGui.QApplication.translate("MainWindow", "Undo (ctrl+z)", None, QtGui.QApplication.UnicodeUTF8))
		self.actionEdit_Settings.setText(QtGui.QApplication.translate("MainWindow", "Edit Settings", None, QtGui.QApplication.UnicodeUTF8))
