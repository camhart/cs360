
class ConfigParser():
	def __init__(self, filepath="web.conf"):
		self.filepath = filepath
		self.hosts = {}
		self.media = {}
		self.parameters = {}

	def parse(self):
		configFile = open(self.filepath)
		try:
			for line in configFile:
				a = line.split()
				if line.startswith('host '):
					self.hosts[a[1]] = a[2]
				elif line.startswith('media '):
					self.media[a[1]] = a[2]
				elif line.startswith('parameter '):
					self.parameters[a[1]] = a[2]
				else:
					if line.strip() == '':
						pass
					else:
						print "Error parsing ",self.filepath," '",line,"'"
		except Exception, e:
			print e
		finally:
			configFile.close()

	def printConfig(self):
		print self.hosts
		print self.media
		print self.parameters

# cp = ConfigParser()
# cp.parse()
# cp.printConfig()
