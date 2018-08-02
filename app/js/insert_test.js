$(document).ready(function() {
  $("#error").css('display', 'none', 'important');
  $('#addTest').submit(function(e) {
    e.preventDefault();
    $.ajax({
      type: "POST",
      url: "processor/insert_test_processor.php",
      data: $(this).serialize(),
      error: function(response, textStatus, XHR) {
        console.log(response);
        console.log("I AM HERE");
        if (response.getResponseHeader('Error-Code') != 200) {
          var cssLink = "css/error.css";
          $("head").append("<link href=" + cssLink + " rel='stylesheet' />");
          $("#error").css('display', 'inline', 'important');
          $("#error_message").css('display', 'inline', 'important');
          $("#stack_trace").css('display', 'inline', 'important');
          $("#error_message").html(response.getResponseHeader('Error-Message'));
          $("#stack_trace").html(response.getResponseHeader('Error-BackTrace'));
        } else {
          var id = response.getResponseHeader('TestID');
          window.location = "aev_test_2.php?TestID=" + id;
        }
      },
      success: function(response, textStatus, XHR) {
        console.log(response);
        console.log(XHR);
        console.log(textStatus);
        var id = XHR.getResponseHeader('TestID');
        console.log("TEST ID = " + id);
        window.location = "aev_test_2.php?TestID=" + id;
      },
      beforeSend: function() {
        $("#error_message").css('display', 'inline', 'important');
        $("#error_message").html("Loading...")
      }
    });

    return false;

  });


  $("#error").css('display', 'none', 'important');
  $('#TestURL').submit(function(e) {
    e.preventDefault();
    $.ajax({
      type: "POST",
      url: "processor/test_url_processor.php",
      data: $(this).serialize(),
      error: function(response, textStatus, XHR) {
        console.log(response);
        if (response.getResponseHeader('Error-Code') != 200) {
          var cssLink = "css/error.css";
          $("head").append("<link href=" + cssLink + " rel='stylesheet' />");
          $("#error").css('display', 'inline', 'important');
          $("#error_message").css('display', 'inline', 'important');
          $("#stack_trace").css('display', 'inline', 'important');
					var URLReturned = response.match(/(?:"[^"]*"|^[^"]*$)/)[0].replace(/"/g, "").replace(/Redirecting to /g,'');
          $("#error_message").html(URLReturned);
          $("#stack_trace").html(response.getResponseHeader('Error-BackTrace'));
        }
      },
      success: function(response, textStatus, XHR) {
        console.log(response);
          var cssLink = "css/error.css";
          $("head").append("<link href=" + cssLink + " rel='stylesheet' />");
          $("#error").css('display', 'inline', 'important');
          $("#error_message").css('display', 'inline', 'important');
					var URLReturned = response.match(/(?:"[^"]*"|^[^"]*$)/)[0].replace(/"/g, "").replace(/Redirecting to /g,'');
          $("#error_message").html(URLReturned);
      },
      beforeSend: function() {
        $("#error_message").css('display', 'inline', 'important');
        $("#error_message").html("Loading...")
      }
    });
    return false;
  });


});
