#!/usr/bin/env python
# -*- coding: utf-8 -*-

import joblib
import numpy as np

def localizeTheCat(rssis):
    # labels = room ids
    labels = loaded_model.classes_.tolist()

    # where is the cat - location
    where_is_a_cat = loaded_model.predict(rssis)[0].tolist()

    # probabilities for each room
    where_is_a_cat_p = loaded_model.predict_proba(rssis)[0].tolist()

    # prediction probability:
    location_p = round(where_is_a_cat_p[labels.index(where_is_a_cat)] * 100, 1)
    return (where_is_a_cat, location_p)

# -------------------------------------------------------------------- #

if __name__ == "__main__":
    # load models
    filename = '3-models/20200118-MLP.pkl'
    loaded_model = joblib.load(filename)

    # input RSSI array, fetch from the database or file with last measurements
    rssis = np.array([[-70, -70, -30, -110]])

    # let's make a prediction!
    where_is_a_cat, location_p = localizeTheCat(rssis)

    # and output the results
    print("The cat is in the %s with p=%s%%" % (where_is_a_cat, location_p))
