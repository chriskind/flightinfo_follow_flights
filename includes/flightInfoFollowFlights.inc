<?php

class flightInfoFollowFlights {
  
  public static $form;
  public static $form_state;
  
  public $flight_id = NULL;
  public $client_id = NULL;
  
  public $active_flightinfo_client_tab_l = NULL;
  public $update = NULL;
  
  public $q;
  public $q_vars;
  
  public $flight_information;
  
  function __construct($form, $form_state) {
  
    self::$form = $form;
    self::$form_state = $form_state;
    $this->add_q_vars();
    $this->add_flight_search();
    
    if(self::$form_state['storage']['step'] == 'step_select_flights') {
      $this->email_or_phone_required();
    }
    
    array_unshift(self::$form['#validate'], 'flight_follower_duplicate_check_validator');
    
    if (!isset(self::$form['#after_build'])) {
      $form['#after_build'] = array();
    }
    
    self::$form['#after_build'][] = 'flightinfo_follow_flights_form_after_build';
    
    flight_follower_searchdate();
    
    if(!empty($this->flight_id)) {
      $this->flight_information = flight_information($this->flight_id);
      $message = "Set up Alerts for: " . $this->flight_information['client_airline']['airline']  . " " . $this->flight_information['client_airline']['flight#'] . " "  . $this->flight_information['from']['from'] . ' (' . $this->flight_information['from']['from_code'] . ") -to- " .  $this->flight_information['to']['to'] . ' (' . $this->flight_information['to']['to_code'] . ')' . $this->flight_information['schedule']['depart_local_2'];
      $node_title = '<div class="flight-follow-title"><h3>' . "Set up Alerts for:</br>" . $this->flight_information['client_airline']['airline']  . " " . $this->flight_information['client_airline']['flight#'] . "</h3><h6>"  . $this->flight_information['from']['from'] . ' (' . $this->flight_information['from']['from_code'] . ") -to- " .  $this->flight_information['to']['to'] . ' (' . $this->flight_information['to']['to_code'] . ')' . "</br>" . $this->flight_information['schedule']['depart_local_2'] . "</h6></div>";
      
      drupal_set_title($node_title, PASS_THROUGH);
    } else {
      $node_title = '<div class="flight-follow-title"><h3>Edit My Flights</h3></div>';
      drupal_set_title($node_title, PASS_THROUGH);
    }
  }
  
  function get_form() {
    // return to drupal
    $return = array();
    $return['form'] = self::$form;
    $return['form_state'] = self::$form_state;
    return($return);
  }
  
  //  TO DO: Move tabs and form to AJAX
  function add_q_vars() {
    $this->q = drupal_get_query_parameters();

    $this->q_vars = array(
      'active_flightinfo_client_tab_l', 
      'flight_id', 
      'client_id',
      'update'
    );
    
    foreach($this->q_vars as $q_var) {
      
	  if(isset($this->q[$q_var])) {
        $this->$q_var = $this->q[$q_var];
      } elseif(isset(self::$form_state['values'][$q_var])) {
        $this->$q_var = $this->q[$q_var];
      } 
    }
  }
  
  function add_flight_search() {
    
    // TO DO: THIS IS A WORK AROUND FOR THE ENTITY REFERENCE VIEW FORM NOT WORKING
    // add search for flight and remove search date
   
    self::$form['flight_id'] = array(
        '#type' => 'hidden', 
        '#value' => $this->flight_id, 
        '#default_value' => 'unset'
    );
    
    self::$form['client_id'] = array(
        '#type' => 'hidden', 
        '#value' => $this->client_id, 
        '#default_value' => 'unset'
    );
    
    $defaults = self::$form['field_anon_choose_flights']['und']['#default_value'];
    $options = self::$form['field_anon_choose_flights']['und']['#options'];
    
    if(isset($this->flight_id)) {
      
      $flight_id_html = self::$form['field_anon_choose_flights']['und']['#options'][$this->flight_id];
      $option = array(
          $this->flight_id => $flight_id_html
      );
      
      if(isset(self::$form['field_anon_choose_flights']['und']['#options'][$this->flight_id])) {
        unset(self::$form['field_anon_choose_flights']['und']['#options'][$this->flight_id]);
      }
      $options = $option + $options;
      self::$form['field_anon_choose_flights']['und']['#options'] = $options;
      
    }
    
    
    if(in_array($this->flight_id, $defaults)) {
      // drupal_set_message('You are already following flight #' . $this->flight_id . ' No additional flights were added');
    } else {
      array_unshift($defaults, $this->flight_id);
    }
    
    
    self::$form['field_anon_choose_flights']['und']['#default_value'] = $defaults;
    self::$form['field_anon_choose_flights']['und']['#options'] = $options;
    
  }

  function email_or_phone_required() {
    
    // $q = implode('&', $this->q);
    $path = drupal_get_path_alias();
    
    $email = self::$form_state['values']['field_anon_address']['und'][0]['email'];
    $phone = self::$form_state['input']['field_anon_phone_number']['und'][0]['number'];
    if(empty($email) && empty($phone)) {
      
      /*
       * TODO this should probably go through form_set_error / form_get_errors pattern
       */
      
      drupal_set_message('You must enter a valid email OR a valid phone number to proceed', 'error');
      $this->q['step'] = 'step_email';
      $q = array('query' => $this->q);
      // adhd_buffer_to_dsm($this->q);
      
      drupal_goto($path, $q);
    }  
  }
  
  function duplicate_check_validator() {
    
  }
  
}