#!/usr/bin/env python3
"""
UEVR Danish Content Detection Script

This script systematically scans all text files in the UEVR project to identify
Danish content that needs translation to English.

Features:
- Recursive file scanning
- Danish character pattern detection
- Common Danish word detection
- Context extraction
- CSV report generation
- Priority assignment
"""

import os
import re
import csv
import argparse
from pathlib import Path
from typing import List, Dict, Tuple
import sys

# Danish character patterns
DANISH_CHARS = r'[æøåÆØÅ]'

# Common Danish words and phrases (technical context)
DANISH_WORDS = [
    'analyser', 'implementering', 'funktionalitet', 'komponenter',
    'håndtering', 'fejl', 'løsning', 'målet', 'næste', 'skridt',
    'ændret', 'følgende', 'manglende', 'oversigt', 'status',
    'genereret', 'antal', 'filer', 'mappe', 'sti', 'størrelse',
    'sidst', 'oprettet', 'gemt', 'kør', 'vent', 'se', 'resultater',
    'hovedresultat', 'hvis du får fejl', 'kommando', 'sekvens',
    'komplet', 'hurtig', 'start', 'system', 'trin', 'gå til',
    'kør master filen', 'færdiggørelse', 'specialiserede',
    'kategorier', 'markdown format', 'samlet', 'dybe tallerken',
    'projekter', 'fulde filer', 'færdig', 'komplette', 'deepwiki',
    'komplette analyser', 'specialiserede analyser',
    'komplet markdown format', 'manglende funktions oversigt',
    'samlet deepwiki', 'dybe tallerken for alle projekters fulde filer',
    'kildekoder', 'uden deps', 'alle', 'komplet', 'system',
    'arkitektur', 'modulær', 'skalerbar', 'framework', 'adapter',
    'hooks', 'mods', 'core', 'utility', 'imgui', 'vr', 'rendering',
    'engine', 'spil', 'kompatibilitet', 'integration', 'performance',
    'optimering', 'sikkerhed', 'test', 'build', 'deployment',
    'monitoring', 'produktion', 'udvikling', 'setup', 'konfiguration',
    'API', 'referencer', 'vejledning', 'fejlfinding', 'FAQ',
    'bidrag', 'licens', 'disclaimer', 'implementering', 'struktur',
    'transformation', 'organisering', 'vedligeholdelse', 'skalerbarhed',
    'afhængigheder', 'namespace', 'linking', 'runtime', 'crashes',
    'memory leaks', 'API compatibility', 'documentation gaps',
    'test coverage', 'build optimization', 'development speed',
    'status rapport', 'aktuelle tilstand', 'planlægger', 'næste skridt',
    'målet om', 'komplet implementering', 'fungerende implementering',
    'avanceret', 'cross-engine', 'VR modding framework', 'understøtter',
    'multiple game engines', 'enkelt platform', 'VR integration',
    'klart ansvar', 'specifikt formål', 'nem at tilføje',
    'nye komponenter', 'uafhængigt', 'dependency management',
    'klare afhængigheder', 'kortsigtet', 'mellemlangt', 'langtid',
    'implementere', 'manglende komponenter', 'systematisk',
    'komplette struktur', 'guide', 'udviklere',
    'alle nødvendige oplysninger', 'arbejde med', 'nye struktur',
    'implementere nye funktioner'
]

# File extensions to scan
TEXT_EXTENSIONS = {
    '.md', '.txt', '.cpp', '.hpp', '.h', '.c', '.lua', '.ps1', '.bat',
    '.py', '.json', '.xml', '.yml', '.yaml', '.ini', '.cfg', '.conf'
}

# Priority mapping based on file type and location
PRIORITY_MAPPING = {
    'README': 'High',
    'tools/': 'High',
    'docs/': 'Medium',
    'src/': 'Low',
    'profiles/': 'Low',
    'promts/': 'Low',
    'cmake/': 'Low',
    'data/': 'Low'
}

class DanishContentDetector:
    """Detects Danish content in UEVR project files."""
    
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.findings = []
        
    def scan_file(self, file_path: Path) -> List[Dict]:
        """Scan a single file for Danish content."""
        findings = []
        
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                lines = content.split('\n')
                
                for line_num, line in enumerate(lines, 1):
                    # Check for Danish characters
                    if re.search(DANISH_CHARS, line):
                        findings.append({
                            'type': 'Danish Characters',
                            'line': line_num,
                            'content': line.strip(),
                            'context': self._extract_context(lines, line_num)
                        })
                    
                    # Check for Danish words
                    for danish_word in DANISH_WORDS:
                        if danish_word.lower() in line.lower():
                            findings.append({
                                'type': 'Danish Word',
                                'line': line_num,
                                'content': line.strip(),
                                'context': self._extract_context(lines, line_num),
                                'danish_word': danish_word
                            })
                            
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            
        return findings
    
    def _extract_context(self, lines: List[str], line_num: int, context_lines: int = 2) -> str:
        """Extract context around a line."""
        start = max(0, line_num - context_lines - 1)
        end = min(len(lines), line_num + context_lines)
        context_lines_list = lines[start:end]
        return '\n'.join(context_lines_list)
    
    def _get_priority(self, file_path: Path) -> str:
        """Determine priority based on file location and type."""
        file_path_str = str(file_path)
        
        for pattern, priority in PRIORITY_MAPPING.items():
            if pattern in file_path_str:
                return priority
        
        return 'Low'
    
    def _get_file_type(self, file_path: Path) -> str:
        """Determine file type based on extension and location."""
        ext = file_path.suffix.lower()
        
        if ext in ['.cpp', '.hpp', '.h', '.c']:
            return 'Source Code'
        elif ext in ['.md', '.txt']:
            return 'Documentation'
        elif ext in ['.ps1', '.bat', '.py']:
            return 'Script'
        elif ext in ['.json', '.xml', '.yml', '.yaml', '.ini', '.cfg', '.conf']:
            return 'Configuration'
        else:
            return 'Other'
    
    def scan_project(self) -> None:
        """Scan entire project for Danish content."""
        print(f"Scanning project: {self.project_root}")
        
        for file_path in self.project_root.rglob('*'):
            if file_path.is_file() and file_path.suffix.lower() in TEXT_EXTENSIONS:
                # Skip translation workspace
                if 'translation-workspace' in str(file_path):
                    continue
                    
                findings = self.scan_file(file_path)
                
                if findings:
                    for finding in findings:
                        self.findings.append({
                            'file_path': str(file_path.relative_to(self.project_root)),
                            'file_type': self._get_file_type(file_path),
                            'priority': self._get_priority(file_path),
                            'finding_type': finding['type'],
                            'line_number': finding['line'],
                            'content': finding['content'],
                            'context': finding['context'],
                            'danish_word': finding.get('danish_word', '')
                        })
                        
                    print(f"Found {len(findings)} Danish content items in {file_path}")
    
    def generate_csv_report(self, output_file: str) -> None:
        """Generate CSV report of findings."""
        with open(output_file, 'w', newline='', encoding='utf-8') as csvfile:
            fieldnames = [
                'File Path', 'File Type', 'Priority', 'Finding Type',
                'Line Number', 'Content', 'Context', 'Danish Word'
            ]
            
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            
            for finding in self.findings:
                writer.writerow({
                    'File Path': finding['file_path'],
                    'File Type': finding['file_type'],
                    'Priority': finding['priority'],
                    'Finding Type': finding['finding_type'],
                    'Line Number': finding['line_number'],
                    'Content': finding['content'],
                    'Context': finding['context'],
                    'Danish Word': finding['danish_word']
                })
        
        print(f"CSV report generated: {output_file}")
    
    def generate_summary(self) -> None:
        """Generate summary of findings."""
        total_findings = len(self.findings)
        high_priority = len([f for f in self.findings if f['priority'] == 'High'])
        medium_priority = len([f for f in self.findings if f['priority'] == 'Medium'])
        low_priority = len([f for f in self.findings if f['priority'] == 'Low'])
        
        file_types = {}
        for finding in self.findings:
            file_type = finding['file_type']
            file_types[file_type] = file_types.get(file_type, 0) + 1
        
        print("\n" + "="*60)
        print("DANISH CONTENT DETECTION SUMMARY")
        print("="*60)
        print(f"Total Findings: {total_findings}")
        print(f"High Priority: {high_priority}")
        print(f"Medium Priority: {medium_priority}")
        print(f"Low Priority: {low_priority}")
        print("\nFindings by File Type:")
        for file_type, count in sorted(file_types.items()):
            print(f"  {file_type}: {count}")
        print("="*60)

def main():
    parser = argparse.ArgumentParser(description='Detect Danish content in UEVR project')
    parser.add_argument('--project-root', default='.', help='Project root directory')
    parser.add_argument('--output', default='danish_content_report.csv', help='Output CSV file')
    
    args = parser.parse_args()
    
    detector = DanishContentDetector(args.project_root)
    detector.scan_project()
    detector.generate_csv_report(args.output)
    detector.generate_summary()

if __name__ == '__main__':
    main()
