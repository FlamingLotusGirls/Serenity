var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Serenity Admin Dashboard' });
});

/* Other frontend-facing page routes go here */

// Add API routes
router.use('/api', require('./api'));

module.exports = router;
