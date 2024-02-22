from http import cookies
import os

# Create a cookie
cookie = cookies.SimpleCookie()
cookie['test_cookie'] = 'test_value'
cookie['test_cookie']['path'] = '/'
cookie['test_cookie']['max-age'] = 86400 * 30  # 30 days

print(cookie, '\r')  # Sends the HTTP header

# Get the cookie value
if 'HTTP_COOKIE' in os.environ:
   received_cookies = cookies.SimpleCookie(os.environ['HTTP_COOKIE'])
   if 'test_cookie' in received_cookies:
       print("Content-Type: text/html\r")
       print("\r")
       print(f"Cookie 'test_cookie' is set!<br>")
       print(f"Value is: {received_cookies['test_cookie'].value}")
else:
	print("Content-Type: text/html\r")
	print("\r")
	print("Cookie named 'test_cookie' is not set!")
if 'HTTP_COOKIE' in os.environ:
	received_cookies = cookies.SimpleCookie(os.environ['HTTP_COOKIE'])
	# print recieved_cookies['test_cookie'] only available onces
	# i dont know the key value so loop over it programatically
	for key in received_cookies.keys():
		print(f"key: {key}")
		print(f"value: {received_cookies[key].value}")
		print(f"expires: {received_cookies[key]['expires']}")
		print(f"max-age: {received_cookies[key]['max-age']}")
		print(f"version: {received_cookies[key]['version']}")
		print(f"path: {received_cookies[key]['path']}")
		print(f"comment: {received_cookies[key]['comment']}")
		print(f"domain: {received_cookies[key]['domain']}")
		print(f"secure: {received_cookies[key]['secure']}")
		print(f"httponly: {received_cookies[key]['httponly']}")
		print(f"rfc2109: {received_cookies[key]['rfc2109']}")
		print(f"port: {received_cookies[key]['port']}")



# print list of os.environ
for key in os.environ.keys():
	print(f"key: {key}")
	print(f"value: {os.environ[key]}")
