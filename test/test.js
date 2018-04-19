var disk = require('../index');

try {
	disk.getDisk('C').then(function(data){
		console.dir(data);
	}).catch(function(err){
		console.error(err);
	});
} catch(e){
	console.error(e);
}