
// Use macro to upcast. In macro verify offsetof() == 0??



// Runs one iteration of the state machine. return true if done running (no transitions left). Return false if still running.
// data is optional - do not null assert.
bool ecu_hsm_run(struct ecu_hsm *me, void *data); 

//polling approach
while (1)
{
    update_data(&data);
    ecu_hsm_run(&me, &data);
}

// event-driven approach
queue_with_events
thread()
{
    block_until_queue_has_event();
    data = queue.pop();
    ecu_hsm_run(&hsm, &data);
}


struct ecu_hsm hsm;

ecu_hsm_ctor(&hsm, &STARTING_STATE);

ecu_hsm_dispatch(&hsm, &event);

ecu_hsm_add_state()


enum ecu_hsm_status STARTING_STATE(struct ecu_hsm *me, struct my_event *e)
{
    switch (e->evt)
    {
        case BUTTON_PRESS_EVT:
        {
            break;
        }

        case TIMEOUT_EVT:
        {
            ecu_hsm_transition_to_state(&IDLE_STATE);
            break;
        }
    }
}



1. ecu_hsm struct contains top state?





// Documentation
1. doxygen
2. doxysphinx build doc/ doc/build Doxyfile
3. sphinx-build -b html doc/ doc/build

pip install sphinx_rtd_theme
pip install doxysphinx
pip install sphinxcontrib-doxylink
pip install sphinx-tabs
pip install sphinx-hoverxref



clang-format --Werror --dry-run -fallback-style=none --ferror-limit=0 --files=files to process --style=file 
