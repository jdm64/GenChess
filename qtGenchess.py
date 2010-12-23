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
color = ["White", "Black"]

def boardIndex(alg):
	ret = ord(alg[0]) - ord('a')
	ret += 8 * (8 - (ord(alg[1]) - ord('0')))
	return ret

def boardCord(index):
	files = ["a", "b", "c", "d", "e", "f", "g", "h"]

	one = files[index % 8]
	two = 8 - (index / 8)
	return one + str(two)

def dropWord(string, num=1):
	return ' '.join(string.split()[num:])

class MainWindow(QMainWindow, Ui_MainWindow):
	def __init__(self, parent=None):
		super(MainWindow, self).__init__(parent)
		self.setupUi(self)

		self.piece = ["P", "N", "B", "R", "Q", "K"]
		self.callList = []
		self.timer = QTimer()

		self.initGui()

		cmdline = ["./genchess", ""]
		self.engine = Process(cmdline)
		self.newGame()

	def __del__(self):
		pass

	def initGui(self):
		self.connect(self, SIGNAL('triggered()'), self.closeEvent)
		self.connect(self.actionNew_Game_F2, SIGNAL("triggered()"), self.newGame)
		for i in range(6):
			self.connect(self.whiteButtons[i], SIGNAL("clicked()"), lambda n="w" + str(i): self.runButton(n))
			self.connect(self.blackButtons[i], SIGNAL("clicked()"), lambda n="b" + str(i): self.runButton(n))
		for i in range(64):
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
		self.engine.write("quit\n")
		sleep(0.2)
		self.engine.terminate()

	def comExe(self, data, lines):
		self.engine.write(data)
		output = self.engine.read()
		while len(output.splitlines()) < lines:
			sleep(0.05)
			output += self.engine.read()
		return output

	def displayMessage(self, msg):
		self.statusbar.showMessage(msg)

	def updateBoard(self, move):
		color = ["W", "B"]
		color = color[self.curr]
		if move[0] in self.piece:
			to = boardIndex(move[1:])
			self.board[to] = color + move[0]
			self.boardButtons[to].setIcon(self.iconMap[self.board[to]])
			return
		fr = boardIndex(move[0:2])
		to = boardIndex(move[2:4])

		self.board[to] = self.board[fr]
		self.boardButtons[fr].setIcon(self.iconMap["EE"])
		self.boardButtons[to].setIcon(self.iconMap[self.board[to]])

	def runComputer(self):
		data = self.comExe("go\n", 2).splitlines()
		move = data[0].split()[1]
		results = data[1].split()[0]

		self.updateBoard(move)

		msg = color[self.curr] + "'s move: " + move
		if results == 'ok':
			self.displayMessage(msg)
		elif results == 'special':
			self.displayMessage(msg + ' (' + dropWord(data[1]) + ')')
		elif results == 'result':
			self.displayMessage(msg + ' ' + dropWord(data[1], 2))
			return
		self.curr ^= True
		self.callList = []
		self.timer.singleShot(300, self.nextPlayer)

	def nextPlayer(self):
		if player[self.curr] == 'computer':
			self.runComputer()

	def newGame(self):
		self.engine.write("newgame\n")
		for i in range(64):
			self.boardButtons[i].setIcon(self.iconMap["EE"])
		self.curr = False
		self.board = [''] * 64
		self.nextPlayer()

	def runButton(self, n):
		if n[0] == "w" or n[0] == "b":
			if len(self.callList) == 1 and self.callList[0] in self.piece:
				self.callList = [ self.piece[int(n[1:])] ]
				self.displayMessage("active button: " + self.callList[0])
				return
			self.callList.append(self.piece[int(n[1:])])
		else:
			self.callList.append(boardCord(int(n[1:])))
		if len(self.callList) == 1:
			self.displayMessage("active button: " + self.callList[0])
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
		self.callList = []

		data = self.comExe("move " + act + '\n', 1)
		results = data.split()[0]

		if results == 'ok':
			self.displayMessage(color[self.curr] + "'s move: " + act)
			self.updateBoard(act)
			self.curr ^= True
		elif results == 'special':
			self.displayMessage(color[self.curr] + "'s move: " + act + " (" + dropWord(data) + ")")
			self.updateBoard(act)
			self.curr ^= True
		elif results == 'illegal':
			self.displayMessage(data)
		elif results == 'result':
			self.displayMessage(color[self.curr] + "'s move: " + act + " " + dropWord(data, 2))
			self.updateBoard(act)
			return
		self.timer.singleShot(300, self.nextPlayer)


if __name__ == "__main__":
	import sys
	app = QApplication(sys.argv)
	win = MainWindow()
	win.show()
	app.exec_()
