git add .

if [ -z "$1"]
then
	echo "No commit message found"
	exit 1
else
	git commit -m "$1"
fi
git push
