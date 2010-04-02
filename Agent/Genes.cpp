// Genes
// -------------------------------------------------------------------
// Copyright (C) 2009 Anders Nissen
//
// This program is free software; It is covered by the GNU General
// Public License version 2 or any later version.
// See the GNU General Public License for more details (see LICENSE).
//--------------------------------------------------------------------

#include "Genes.h"

#include <iostream>

#include <stdlib.h> // For rand() and RAND_MAX

using std::map;
using std::string;
using std::cout;
using std::endl;

namespace Motivation {

    Genes::Genes() {

    }

    Genes::~Genes() {

    }

    void Genes::SetValue(string id, float value) {
        if (value < 0.0) {
            cout << "[Genes::SetValue] Warning: Clamping id '" << id << "' of value " << value << " to 0.0" << endl;
            value = 0.0;
        } else if (value > 1.0) {
            cout << "[Genes::SetValue] Warning: Clamping id '" << id << "' of value " << value << " to 1.0" << endl;
            value = 1.0;
        }
        gene_map[id] = value;
    }

    float Genes::GetValue(string id) {
        map<string,float>::iterator gene_iter = gene_map.find(id);
        if (gene_iter == gene_map.end()) {
            cout << "[Genes::GetValue] Warning: Id '" << id << "' doesn't exist!" << endl;
        }
        return (*gene_iter).second;
    }

    float Genes::GetInverseValue(string id) {
        return (1.0 - GetValue(id));
    }

    Genes* Genes::Combine(Genes* g) {
        return Crossover(g)->Mutate();
    }

    Genes* Genes::Crossover(Genes* g) {
        Genes* g2 = new Genes();
        for (map<string,float>::iterator gene_iter = gene_map.begin(); gene_iter != gene_map.end(); gene_iter++ ) {
            string id  = (*gene_iter).first;
            float val1 = (*gene_iter).second;
            float val2 = g->GetValue(id);
            float randVal = (float)rand()/RAND_MAX;
            if (randVal < 0.5) {
                g2->SetValue(id, val1);
            } else {
                g2->SetValue(id, val2);
            }
        }
        return g2;
    }

    Genes* Genes::Mutate(float rate) {
        for (map<string,float>::iterator gene_iter = gene_map.begin(); gene_iter != gene_map.end(); gene_iter++ ) {
            float ran = ((float)rand()/RAND_MAX);
            if (ran < rate) {
                string id       = (*gene_iter).first;
                float new_value = ((float)rand()/RAND_MAX);
                SetValue(id, new_value);
            }
        }
        return this;
    }

    void Genes::Print() {
        cout.precision(3); // Setting the output float precision
        cout << "[";
        for (map<string,float>::iterator gene_iter = gene_map.begin(); gene_iter != gene_map.end(); gene_iter++ ) {
            if (gene_iter != gene_map.begin()) cout << ", ";
            cout << (*gene_iter).first << ": " << (*gene_iter).second;
        }
        cout << "]";
    }

    Genes* Genes::GetRandomGenes() {
        Genes* g = new Genes();
        for (map<string,float>::iterator gene_iter = gene_map.begin(); gene_iter != gene_map.end(); gene_iter++ ) {
            string id    = (*gene_iter).first;
            float  value = ((float)rand()/RAND_MAX);
            g->SetValue(id, value);
        }

        return g;
    }

    float Genes::GetAverage() {
        float totalValue = 0.0;
        for (map<string,float>::iterator gene_iter = gene_map.begin(); gene_iter != gene_map.end(); gene_iter++ ) {
            totalValue += (*gene_iter).second;
        }
        return (totalValue / GetGeneCount());
    }

    int Genes::GetGeneCount() {
        return gene_map.size();
    }
}
