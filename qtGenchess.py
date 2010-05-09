#!/usr/bin/env python
# -*- coding: utf-8 -*-

from asyncproc import Process
from time import sleep
from PyQt4.QtCore import SIGNAL, QSize, QTimer
from PyQt4.QtGui import QMainWindow, QApplication, QIcon
from pychess_ui import Ui_MainWindow
from os import listdir

# Edit this to change who plays white/black
# First item is white, second is black
player = ["human", "computer"]

class MainWindow(QMainWindow, Ui_MainWindow):
	def __init__(self, parent=None):
		super(MainWindow, self).__init__(parent)
		self.setupUi(self)

		self.piece = ["P", "N", "B", "R", "Q", "K"]
		self.files = ["a", "b", "c", "d", "e", "f", "g", "h"]
		self.board = [''] * 64
		self.callList = []
		self.buff = ''
		self.curr = False
		self.timer = QTimer()

		self.initGui()

		cmdline = ["./genchess", "-X", "-w" + player[0], "-b" + player[1]]
		self.genchessExe = Process(cmdline)
		self.nextPlayer()

	def initGui(self):
		self.connect(self, SIGNAL('triggered()'), self.closeEvent)
		self.connect(self.actionNew_Game_F2, SIGNAL("triggered()"), self.newGame)
		for i in range(0, 6):
			self.connect(self.whiteButtons[i], SIGNAL("clicked()"), lambda n="w" + str(i): self.runButton(n))
			self.connect(self.blackButtons[i], SIGNAL("clicked()"), lambda n="b" + str(i): self.runButton(n))
		for i in range(0, 64):
			self.connect(self.boardButtons[i], SIGNAL("clicked()"), lambda n="n" + str(i): self.runButton(n))
		self.iconMap = {}
		list = listdir("./Images/")
		for item in list:
			icon = QIcon()
                	icon.addFile("./Images/" + item)
			self.iconMap[item[0] + item[1]] = icon
		for i in range(0, 6):
			self.whiteButtons[i].setIcon(self.iconMap["W"+self.piece[i]])
			self.blackButtons[i].setIcon(self.iconMap["B"+self.piece[i]])

	def closeEvent(self, event):
		self.genchessExe.write("quit\n")
		sleep(0.2)
		self.genchessExe.terminate()

	def __del__(self):
		pass

	def newGame(self):
		self.genchessExe.write("new\n")
		for i in range(64):
			self.boardButtons[i].setIcon(self.iconMap["EE"])
		self.curr = False
		self.board = [''] * 64
		self.nextPlayer()

	def nextPlayer(self):
		data = self.commExe(wait=False)
		if data == '':
			data = None
		elif data[0] in ['!', '$']:
			self.displayMessage(data)
			data = None
		if player[self.curr] == 'computer':
			self.runComputer(data)
		self.callList = []

	def runComputer(self, move=None):
		if move == None:
			move = self.commExe()
		self.updateBoard(move)
		self.curr ^= True
		self.timer.singleShot(300, self.nextPlayer)

	def runButton(self, n):
		if n[0] == "w" or n[0] == "b":
			self.callList.append(self.piece[int(n[1:])])
		else:
			index = int(n[1:])
			one = self.files[index % 8]
			two = 8 - (index / 8)
			self.callList.append(one + str(two))
		if len(self.callList) == 1:
			self.statusbar.showMessage("active button: " + self.callList[0])
			return
		elif self.callList[1][0] in self.piece:
			self.statusbar.clearMessage()
			self.callList = []
			return
		elif self.callList[0] == self.callList[1]:
			self.statusbar.clearMessage()
			self.callList = []
			return
		act = ''.join(self.callList)
		self.statusbar.showMessage("doing move: " + act)
		self.callList = []

		out = self.commExe(act + '\n')
		if out[0] == '.':
			self.updateBoard(act)
			self.curr ^= True
			self.timer.singleShot(300, self.nextPlayer)
		else:
			self.displayMessage(out)

	def commExe(self, data=None, wait=True):
		if data != None:
			self.genchessExe.write(data)
		sleep(0.03)
		num = len(self.buff.splitlines(1))
		if num == 0:
			self.buff = self.genchessExe.read()
			if wait:
				while self.buff == '':
					sleep(0.03)
					self.buff = self.genchessExe.read()
		elif num == 1:
			out = self.buff
			self.buff = ''
			return out
		num = len(self.buff.splitlines(1))
		if num < 2:
			out = self.buff
			self.buff = ''
			return out
		out, self.buff = self.buff.splitlines(1)[0], self.buff.splitlines(1)[1:]
		self.buff = ''.join(self.buff)
		return out

	def updateBoard(self, act):
		color = ["W", "B"]
		color = color[self.curr]
		if act[0] in self.piece:
			to = self.getBoardIndex(act[1:])
			self.board[to] = color + act[0]
			self.boardButtons[to].setIcon(self.iconMap[self.board[to]])
			return
		fr = self.getBoardIndex(act[0:2])
		to = self.getBoardIndex(act[2:4])

		self.board[to] = self.board[fr]
		self.boardButtons[fr].setIcon(self.iconMap["EE"])
		self.boardButtons[to].setIcon(self.iconMap[self.board[to]])

	def getBoardIndex(self, alg):
		ret = ord(alg[0]) - ord('a')
		ret += 8 * (8 - (ord(alg[1]) - ord('0')))
		return ret

	def displayMessage(self, msg):
		self.statusbar.showMessage(msg[2:])


if __name__ == "__main__":
	import sys
	app = QApplication(sys.argv)
	win = MainWindow()
	win.show()
	app.exec_()
