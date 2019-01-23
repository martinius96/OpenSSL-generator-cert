rem CA cert
openssl genrsa -out myCA.key 2048
openssl req -x509 -config certificate-authority-options.conf -new -nodes -key myCA.key -sha256 -days 1825 -out myCA.pem
openssl x509 -outform pem -in myCA.pem -out myCA.crt

rem server cert
openssl genrsa -out server.key 2048
openssl req -config options.conf -new -key server.key -out server.csr
openssl x509 -req -in server.csr -CA myCA.pem -CAkey myCA.key -CAcreateserial -out server.pem -days 1825 -sha256 -extfile server.ext
openssl x509 -outform pem -in server.pem -out server.crt

rem client cert
openssl genrsa -out client.key 2048
openssl req -config options.conf -new -key client.key -out client.csr
openssl x509 -req -in client.csr -CA myCA.pem -CAkey myCA.key -CAcreateserial -out client.pem -days 1825 -sha256 -extfile client.ext
openssl pkcs12 -inkey client.key -in client.pem -export -out client.pfx