set -e -x
cd build || exit
sudo yum install -y RPMS/x86_64/*.rpm
sudo systemctl status cdcm
sudo yum -y erase cdcm
