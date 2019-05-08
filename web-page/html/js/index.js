const pagesFunctions = {};

function registerPage(name, pagesFunction) {
  pagesFunctions[name] = pagesFunction;
}

$('.page.home-page').show();

$('.page-link').click(function(event) {
  event.preventDefault();
  const name = this.dataset.page;
  $('.page').hide();
  $('.' + name + '-page').show();

  pagesFunctions[name]();
  console.log(location.hostname);
});
