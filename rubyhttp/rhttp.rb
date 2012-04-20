#a tcp server

class Token
	def initialize(key, val)	
		@cat = key
		@value = val
	end
	def category
		@cat
	end
	def value
		@value
	end
	def cat=(type)
		@cat = type
	end
	def value=(value)
		@value = value
	end
end

class Parser
	def initialize
		@tokenList = {'GET' => 130,
				  '.js' => 131,
				  '.html' => 132,
				 }
		@tokens = []
	end
	def parse(client)
		input = client.gets
#		input = self.openClient(client)
		input = input.split if input!=nil
		input.each {|i| 
		tmp = @tokenList.find {|key, val| key == i}
		if tmp != nil
			tok = Token.new(tmp[1], tmp[0])
			@tokens.push(tok)
		else 
			tok = Token.new(135, i)
			@tokens.push(tok)
		end	} 
	end
	def tokens
		@tokens
	end
	def openClient(client)
		line = ""
		lines = ""
		while line = client.gets do
			if line != ""
				lines = lines + line
			end
		end
		return lines
	end
end


class HTTPHandler
	def initialize(request)
		@client = request
		@parser = Parser.new
		@response = [] 
	end
	def parseRequest
		@parser.parse(@client)
	end
	def buildResponse

		req = @parser.tokens
		self.getResponse(req[1]) if req[0].category == 130
	end
	def getResponse(token) 
		path = token.value
		if resource = self.openresource(path)
		@client.print("HTTP/1.0 200 OK\r\nKevServRuby0.1\r\nContent-type : text/html\r\n\r\n")
		@client.print(resource)
		else 
			@client.print("HTTP/1.0 404 Not Found\r\n\r\n")
			@client.print("<!DOCTYPE HTML> <html> <head> <title> 404 not found </title> </head> <body> <h1> 404 NOT FOUND </h1> </body> </html>")
		end
	end
	def openresource(path)
		buf = []
		if path[0] != '.'
			path = '.' + path
		end
		begin
			buf = IO.read(path)
			return buf
		rescue SystemCallError
			return false
		end
	end
end

require 'socket'
label = 0
port =  (ARGV[0] || 80).to_i
server = TCPServer.new('localhost', port)
threads = []
$stderr.puts "server made\n"
loop {
	Thread.start(server.accept) do |session|
		$stderr.puts "another session\n"
		req = HTTPHandler.new(session)
		req.parseRequest
		req.buildResponse
		session.close
	end
	}
