set -e
cd build || exit
sudo yum install -y ~/rpmbuild/RPMS/x86_64/*.rpm
sudo systemctl status cdcm
sudo yum -y erase cdcm
