@echo off
echo ""
echo ".mode csv"
echo ".headers on"
echo ".output result.csv"
echo "select models.id, makers.name, models.name, models.diagonal, models.type, models.filename from models inner join makers on models.makerid = makers.id;"
echo ""
sqlite3 exif.sqlite
pause