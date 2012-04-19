#a tcp server

class Token
	def initialize(key, val)	
		@type = key
		@value = val
	end
	def type
		@type
	end
	def value
		@value
	end
	def type=(type)
		@key = type
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
		input = []
		input = client.gets.split(' ')
		$stderr.puts input
		input.each {|i| 
		if @tokenList.find {|key, val| key == i}
			val = @tokenList[i]
			tok = Token.new(val, i)
			@tokens.unshift(tok)
		else 
			tok = Token.new(135, i)
			@tokens.unshift(tok)
		end	} 
	end
	def tokens
		@tokens
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
		if req[0].type = 130
			self.getResponse(req[1]) #its in the next token
		end
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
		req = HTTPHandler.new(session)
		req.parseRequest
		req.buildResponse
		session.close
	end
	}
