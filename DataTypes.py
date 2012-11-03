#DataTypes module for UBC Sailbot 2013 Control Team
import math

class BoundInt:
	def __init__(self, target = 0, low=0, high=1):
		self.lowerLimit = low
		self.upperLimit = high
		self._value = target
		self._balance()
	
	def _balance(self):
		if (self._value > self.upperLimit):
			self._value = self.upperLimit
		elif (self._value < self.lowerLimit):
			self._value = self.lowerLimit
		self._value = int(round(self._value))

	def value(self):
		return self._value

	def set(self, target):
		self._value = target
		self._balance()

	def __str__(self):
		return str(self._value)

class Angle:
	def __init__(self, target):
		self._degree = target
		self._balance()

	def degree(self):
		return self._degree

	def radian(self):
		return math.radians(self._degree)

	def set(self, target):
		self._degree = target
		self._balance()

	def add(self, target):
		self._degree += target
		self._balance()
	
	def _balance(self):
		while (self._degree < 0):
			self._degree = self._degree + 360
		if (self._degree >= 360):
			self._degree = self._degree % 360

	def __str__(self):
		return str(self._degree)


if (__name__ == "__main__"):
	#INFORMAL unit tests for Bounded Value Class
	#TODO: Formalize
	print "Testing BoundInt Class:"
	x = BoundInt()
	print x
	x.set(2)
	print x
	x = BoundInt(0,-10,10)
	x.set(-20)
	print x
	x.set(20)
	print x

	#INFORMAL unit tests for Angle Class
	#Todo: Formalize
	print "Testing Angle Class"
	x = Angle(0)
	print x
	x.set(370)
	print x
	x.add(10)
	print x
	x.set(-10)
	print x
	x.set(0)
	x.add(-10)
	print x
	x.set(-370)
	print x
	x.set(0)
	print str(x.radian())
	x.set(180)
	print str(x.radian())
	x.set(360)
	print str(x.radian())
	print x